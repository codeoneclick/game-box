# Architecture Issues for AI-Assisted Maintenance

This is a risk inventory, not a refactoring patch. Each item is based on inspected repository evidence and includes a possible future improvement. Priorities describe the chance that a seemingly local change has non-local effects.

## 1. Lazy, order-dependent ECS type IDs

**Risk: high.** `STTI_CLASS_GUID` in `gbCore/sources/main_headers.h` assigns an integer using the size of a global set the first time each component/system type calls `class_guid()`. `ces_entity` uses that integer directly as an index into a 255-element component array and mask. Because assigned IDs start at 1, only IDs 1 through 254 are safely addressable; registering a 255th component type would produce index 255, outside the array/bitset.

Why this is difficult for an AI agent:

- “Registration” is hidden in a macro and happens on first use, not in a visible registry.
- IDs are process-local and depend on first-use order.
- Component and system IDs look similar but use separate base-class sets.
- A new component appears self-registering, so the effective 254-component limit and index invariant are easy to miss.
- Concurrent first use of different types mutates the same `std::set`; the per-type `once_flag` does not visibly serialize the shared set itself.

Future AI-friendly direction: replace/augment the macro with an explicit typed registry that validates capacity, detects duplicates, documents stability, and exposes a dump of names -> IDs. At minimum, add a central registry test and a comment in every component/system template.

## 2. Entity construction has a mandatory hidden factory protocol

**Risk: high.** `ces_entity::construct<T>()` sets `m_self_weak`, executes deferred component constructors, and calls `setup_components()`. Derived constructors such as `game_object_3d` queue components rather than constructing them immediately.

Why difficult: `std::make_shared<Derived>()` compiles and looks idiomatic but produces a partially initialized engine entity. The required protocol is not enforced by access control because derived constructors are public in many classes.

Future direction: make constructors protected/private with factory friendship, or expose a clearly named `create()` API and runtime assertion that initialization completed. Add a unit test showing the required lifecycle.

## 3. ECS query membership is eventually updated

**Risk: high.** Entity/component changes only enqueue feeder events. Mask reference lists are repaired at the next `ces_systems_feeder::on_update()`.

Why difficult: a component can be added and queried through the entity immediately, but systems will not see the entity in `enumerate_entities_with_components()` until the next feeder update. Removal similarly leaves weak/query state pending. This timing contract is implicit.

Future direction: document “structural changes take effect next ECS tick,” add explicit `flush_structural_changes()` for controlled cases, and test add/remove timing.

## 4. System execution order has two dimensions

**Risk: high.** Systems have a numeric `order`, but the feeder runs three global phase loops: all starts, all feeds, all ends. Many demo-added systems never set an order and therefore retain `uint8_t::max()`.

Why difficult: an agent may assume each system executes start/feed/end atomically in order. Equal/default order makes dependencies depend on insertion/list-sort stability. Also, `ces_base_system::set_is_paused()` writes `m_is_paused`, while the feeder checks `can_be_feeded()`, whose base implementation consults only `ces_system_modifiers_component`; the relationship between these pause mechanisms is unclear.

Future direction: define named stages and explicit before/after dependencies, assert duplicate/default orders in debug builds, and unify or document pause semantics.

## 5. Scene graph ownership is split across ordered and unique child containers

**Risk: medium-high.** `ces_entity` strongly owns the same children in an `unordered_set` and a `vector`, with a weak parent. It also owns components, while systems cache weak entity references.

Why difficult: every child mutation must keep two containers and feeder recursion consistent. Iteration order differs depending on whether code uses `children`, `enumerate_children`, or directly iterates internal sets. `add_child()` contains non-obvious parent handling that merits a focused correctness review before changes.

Future direction: use one authoritative ordered container plus an index/set only when measured necessary; centralize attach/detach invariants and test reparenting, duplicate add, recursive feeder assignment, and destruction.

## 6. The render abstraction is partial and OpenGL-shaped

**Risk: high.** Shared code exposes `graphics_context`, `render_pipeline`, resources, and `render_pass_descriptor`, but also calls `gl::command` and stores `gl::constant` values. `material`, buffers, textures, shaders, and `ces_render_system` contain compile-time Vulkan/Metal/OpenGL branches.

Why difficult: the presence of an RHI-looking facade suggests a backend can be changed in one implementation directory, but features often require synchronized edits in common classes and all backend branches. GL names in configuration (`GL_BACK`, `GL_LINEAR`) are translated into a cross-backend state vocabulary without being modeled as backend-neutral enums.

Future direction: define backend-neutral descriptors for buffers, textures, samplers, pipelines, passes, and draw commands; keep GL/Vulkan/Metal translation behind backend implementations. Until then, maintain a checklist of every backend-conditional file.

## 7. Backend selection is hard-coded in a shared header

**Risk: high.** `gbCore/sources/gl_commands.hpp` defines Metal for Apple platforms and Vulkan for Windows, with alternative OpenGL defines commented out.

Why difficult: changing a comment/define changes compilation across the engine and can conflict with project-provided definitions. It is not discoverable from the build command. OpenGL implementations remain in-tree but are not the current default, so they may silently drift.

Future direction: move `USED_GRAPHICS_API` to explicit build configurations, fail compilation if zero or multiple APIs are selected, and continuously compile at least one target per supported backend.

## 8. Backend code is duplicated across common resource classes

**Risk: high.** `material.cpp`, `texture.cpp`, `cubemap_texture.cpp`, `vbo.cpp`, `ibo.cpp`, `shader*`, render techniques, and the render system each contain backend branches. Metal draw-buffer setup is repeated between 2D and 3D paths in `ces_render_system.cpp`.

Why difficult: a fix in one draw path/backend can easily miss its sibling. The files are large, and preprocessor-elided code is not checked by the active build.

Future direction: extract one backend-neutral draw packet and backend command encoder; share 2D/3D buffer/uniform submission helpers; add a backend feature matrix with compile/smoke tests.

## 9. Global state and singleton lifetime are pervasive

**Risk: high.** Examples include graphics current context, Vulkan device/surface/swapchain singletons, Metal device singleton, thread pool singleton, static audio maps/pool, global type-ID sets, GL state caches, technique-name indices, configuration enum maps, and static entity tag counters.

Why difficult: ownership and teardown order are not expressed by constructors. Tests cannot easily isolate state. Multiple windows/contexts/worlds may unintentionally share backend state. An agent may create a second instance that still talks to the first singleton.

Future direction: introduce an engine/runtime context that owns services and is passed to transitions/fabricators; make singleton use explicit and resettable in tests; document destruction order.

## 10. Resource loading has implicit thread-affinity and synchronization assumptions

**Risk: high.** `resource_accessor` serializes operations on a worker thread and commits them during its game-loop update. `sync=true` performs both immediately. Operation status and execution flags are ordinary fields; map iteration/locking is asymmetric, and the worker may sleep while holding the mutex.

Why difficult: “sync” can mean GPU work on the calling thread, which is safe only with the expected graphics context/thread. Callbacks may fire immediately for cache hits but later for new resources. An agent can accidentally move GPU commit off-thread or introduce a data race.

Future direction: formalize thread annotations (`worker serialize`, `render-thread commit`, `main callback`), use condition variables and atomics/locked state transitions, make callback delivery semantics consistent, and test cache-hit/miss async behavior.

## 11. Material creation requires several ordered steps

**Risk: medium-high.** A usable material generally requires typed configuration, `material::construct()`, `set_shader()`, `set_textures()`, and technique attachment through `ces_material_extension`/component. Vulkan pipeline creation additionally depends on a vertex input state at bind time; Metal depends on current pass state.

Why difficult: `construct()` sounds complete but is not. A material can exist while its shader is uncommitted, and the render system silently excludes it via the committed check.

Future direction: add one public material factory returning a clearly staged/ready handle, expose failure diagnostics, and make invalid partial states unrepresentable.

## 12. Render-pass wiring relies on string conventions

**Risk: high.** Transition XML lists WS/SS/output configurations. Material `technique_name` strings and pass GUIDs must correspond. Render code strips text up to the first underscore from technique names, while repository content commonly uses dotted names.

Why difficult: the graph is spread over several XML files and C++ name/index registries. Typos are runtime assertions or missing draws, not schema/link errors. Attachment textures are registered under derived string GUIDs such as `.color`, `.depth`, and named attachments.

Future direction: build and validate a render-graph manifest at transition load, with unique IDs, declared inputs/outputs, cycle/order checks, and diagnostics listing unresolved techniques/textures.

## 13. Generated configuration code is mixed with hand-written code

**Risk: high.** Schema inputs live under `gbCore/configurations` and `gbDemo/configurations`; generated C++ lives under `autogenerated`; runtime XML lives under `gbBundle/Configurations`. The Python generator uses Python 2 syntax and writes C++.

Why difficult: three similarly named layers are easy to confuse. Hand-editing generated files may appear to work until regeneration. The generator itself contains suspicious repeated component assignments in vector parsing, so regeneration needs review rather than blind acceptance.

Future direction: add generated-file banners, a reproducible pinned generator environment, a `generate && git diff --exit-code` CI check, XML schema validation, and an explicit README describing schema vs generated classes vs runtime content.

## 14. Runtime asset lookup and Windows staging are name-based and flattened

**Risk: high.** `gbScripts/build-gbDemo.ps1` copies supported files from hierarchical source roots into one `gbWin32SharedResources` directory. `common_*` path helpers search platform-specific locations/custom paths.

Why difficult: duplicate basenames from different source folders can overwrite each other during staging. Editing the staged copy diverges from `gbBundle`. A file that works on Apple bundle paths may collide on Windows.

Future direction: preserve relative paths in staging, detect collisions before copy, generate a manifest with source/destination hashes, and make all resource GUIDs canonical relative paths.

## 15. Extension dispatch and failures are assertion-heavy

**Risk: medium-high.** Loading operations use substring checks such as `.png`, `.fbx`, `.GB3DMESH`, and assert on unsupported input. Many constructors and configuration reads use `assert` instead of returning structured errors.

Why difficult: behavior changes between debug and release, error context is weak, and substring matching is not equivalent to a normalized extension. AI-generated content mistakes can fail far from the source XML.

Future direction: normalize extensions, use an explicit loader registry, return typed errors with filename/config path/backend, and retain assertions only for internal impossibilities.

## 16. Naming is inconsistent and sometimes misspelled

**Risk: medium.** Examples include `game_conroller_*`, `commiter`, `transfering`, `compoment`, `pathes`, `dependecies`, `pet_queue`, `cammand`, `trashhold`, `game_center_provier`, and `ui_editor_scene..cpp`. Namespace mismatches also exist (database ECS classes under `gb::al`).

Why difficult: agents tend to “correct” names and then break project-file references or ABI/source call sites; searches for conventional spellings miss the implementation.

Future direction: publish a legacy-name glossary, introduce correctly named aliases/wrappers before migrations, and use scripted repository-wide renames with builds for every platform project.

## 17. Build metadata is duplicated and partly stale

**Risk: high.** Source membership and settings are independently encoded in multiple `.vcxproj` and Xcode project files. `gbCore/gbCore.sln` references a missing `gbAnimation` project. Some server project entries reference shared source filenames absent from the current tree. Older dependencies remain in link lists.

Why difficult: adding a `.cpp` can make one platform pass and another silently omit it. The solution file appears authoritative but the current PowerShell build script uses an explicit project list instead.

Future direction: adopt a single source-of-truth build generator (for example CMake only if the maintainers choose it), or at minimum generate/check project file source lists and remove stale references. Document supported target/configuration combinations.

## 18. No automated test suite was found

**Risk: high.** No engine-owned test files/framework configuration were discovered. The demo executable is the practical integration test.

Why difficult: an agent cannot cheaply verify ECS registration order, resource thread behavior, serializer compatibility, render-graph wiring, or backend parity. Compile success leaves large behavioral gaps.

Future direction: start with headless tests for ECS/entity/feeder and configuration parsing; serializer golden files; then backend compile tests and small offscreen/screenshot smoke tests. Add tests before deep refactoring.

## 19. “Navigation” is application route following, not an engine service

**Risk: medium.** Route points come from TMX object groups and live in `ces_level_route_component`; AI uses checkpoint look-ahead. No navmesh, graph search, or core navigation module was found.

Why difficult: a task phrased “modify navigation” could be misrouted into `gbCore` or assume capabilities that do not exist.

Future direction: name the existing feature “racing route AI” in docs/code. If general navigation is later required, specify it as a new subsystem with ownership and data formats.

## 20. Large application fabricators and systems combine many responsibilities

**Risk: medium-high.** `gbDemo/sources/gameplay_fabricator.cpp`, `ces_state_automat_system.cpp`, and UI interaction systems are large and coordinate resource interpretation, entity construction, physics, UI, and gameplay state.

Why difficult: a narrow feature may have multiple implicit setup sites and broad include dependencies. It is hard to isolate canonical behavior from one-off code.

Future direction: split by cohesive product concept (level import, car construction, environment construction, UI state), expose small factory interfaces, and add characterization tests before moving code.

## 21. Public/private module boundaries are informal

**Risk: medium.** All headers live in `sources`; `main_headers.h` imports broad third-party/platform dependencies; consumers frequently reach into concrete core/backend types.

Why difficult: an agent cannot tell whether a header is a supported API or an implementation detail. Changes propagate large recompiles and accidental coupling.

Future direction: define public include directories, internal namespaces/headers, module dependency rules, and a small umbrella API per library.

## 22. Uncertainties that need dedicated investigation

- `UNKNOWN`: supported, currently green Xcode versions/targets and whether all Metal paths compile.
- `UNKNOWN`: intended semantics of `__NO_RENDER__` versus `NO_GRAPHICS_API`; inspect all project preprocessor settings and preprocessing output.
- `UNKNOWN`: swapchain/window resize and device-loss behavior; no complete recreation path was identified.
- `UNKNOWN`: reliable shutdown order among job, resource, audio, network, graphics, and native application objects.
- `UNKNOWN`: current server build viability because project references do not match the tree.
- `UNKNOWN`: intended status of OpenGL support after Windows switched to Vulkan and Apple to Metal.
- `UNKNOWN`: serialization compatibility/versioning guarantees for GB3DMESH, GB3DANIM, and GB3DSCENE.

Each unknown should be resolved with a focused build/runtime task rather than guessed during an unrelated feature change.
