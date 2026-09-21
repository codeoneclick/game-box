# Draft AGENTS.md

> This is a proposed repository instruction file derived from the codebase audit. Rename/copy it to `AGENTS.md` only after maintainer review.

## Repository purpose

This repository contains the Game Box C++ engine and a racing-game demo. The engine uses an ECS plus a hierarchical scene graph. It supports compile-time OpenGL, Vulkan, and Metal rendering paths; the current selectors choose Vulkan on Windows and Metal on macOS/iOS/tvOS.

Read these architecture documents before engine work:

- `ENGINE_MAP.md` — subsystem and ownership map.
- `ENGINE_FLOWS.md` — runtime/data-flow diagrams.
- `CODE_PATTERNS.md` — canonical examples.
- `ARCHITECTURE_ISSUES.md` — known hazards and uncertainties.

## Module boundaries

- `gbCore/`: reusable engine runtime (ECS, lifecycle, rendering, resources, scenes, input, Box2D integration, jobs, configuration).
- `gbUI/`: reusable ECS-based UI controls.
- `gbSound/`: audio engine and ECS sound bridge.
- `gbDatabase/`: SQLite wrappers; application tables remain in the application.
- `gbNetwork/`: ASIO transport, commands, and network ECS integration.
- `gbDemo/`: racing-game application and primary end-to-end sample.
- `gbServer/`: server/sample executable; build metadata may be stale.
- `gb_client_server_shared/`: sources compiled directly by client/server projects.
- `gbBundle/`: canonical runtime asset sources.
- `gbWin32SharedResources/`: generated/flattened Windows runtime staging. Do not treat it as the canonical asset source.
- `gbScripts/`: builds, configuration generation, shader/content tools, and asset staging.
- `gbCore/libraries/` and `gbDemo/Pods/`: vendored dependencies. Do not edit unless the task explicitly targets a dependency.

Put game-specific behavior in `gbDemo` (or the relevant executable) and reusable behavior in the owning engine module. Do not add application rules to `gbCore` just because core types are convenient.

## Before editing

1. Run `git status --short` and preserve all pre-existing user changes. This repository often has generated/staged resource changes.
2. Locate the owning subsystem and a canonical example in `CODE_PATTERNS.md`.
3. Trace both the MSBuild and Xcode project membership for any new source file.
4. For rendering changes, identify every affected backend branch before editing.
5. If behavior is uncertain, mark it `UNKNOWN` in analysis and cite the files to inspect; do not invent APIs.

## ECS rules

- Construct entities with `gb::ces_entity::construct<T>(...)`. Do not use `std::make_shared<T>()` for derived engine entities that rely on deferred/default components.
- Components derive from `ces_base_component` and declare `STTI_CLASS_GUID(Component, ces_base_component::g_guids_container)` (qualified as needed).
- Systems derive from `ces_base_system` and declare `STTI_CLASS_GUID(System, ces_base_system::g_guids_container)`.
- Build query masks in the system constructor with `add_required_component_guid()` followed by `add_required_components_mask()`.
- Query through `enumerate_entities_with_components()`; use direct `get_component<T>()` for an already known entity.
- Register systems through the transition. Set an explicit order after checking existing transition and scene registrations.
- Structural changes are queued: system query membership is repaired on the next feeder update.
- Do not change `STTI_CLASS_GUID`, the 255-slot component storage, feeder phase ordering, or entity attach/detach behavior as part of an unrelated task.

## Scene and lifecycle rules

- A runtime world is a `scene_graph` subclass owned by a `game_transition`.
- A transition implements `create_scene()`/`destroy_scene()` and is selected by `game_controller` using a transition configuration filename/GUID.
- Scene `create()` installs cameras, scene-specific systems, factories, and child entities.
- `scene_2d` and `scene_3d` are resource assets, not active scene roots. Application fabricators interpret them into runtime ECS entities.
- Use `scene_fabricator` for reusable configured objects, `ui_fabricator` for reusable controls, and application fabricators for game-specific assembly.

## Rendering rules

- Backend selection is in `gbCore/sources/gl_commands.hpp`; treat changes there as repository-wide build changes.
- Normal game rendering uses transformation + geometry + material components and configured technique names. Do not add one-off drawing to `ces_render_system`.
- Prefer material/shader/technique XML under `gbBundle/Configurations` and shader pairs under `gbBundle/Shaders` for visual features.
- External shaders are paired `<base>.vert` and `<base>.frag`, referenced from material XML as `<base>` without an extension.
- Render passes are composed by transition XML: ordered WS geometry passes, ordered SS fullscreen passes, and one output pass.
- OpenGL, Vulkan, and Metal are only partially abstracted. Changes to materials, buffers, textures, shaders, passes, or presentation must be checked against all supported backend branches.
- Do not move GPU resource commit work to the resource worker thread.

## Assets and generated configuration

- Canonical assets live under `gbBundle`; Windows staging is produced by `gbScripts/build-gbDemo.ps1` or `win32.resources.copy.bat`.
- Runtime configuration XML lives under `gbBundle/Configurations`.
- Configuration schema inputs live under `gbCore/configurations` and `gbDemo/configurations`.
- Generated typed configuration code lives under `gbCore/autogenerated` and `gbDemo/autogenerated`.
- Do not hand-edit `autogenerated` files. Change the sample schema and rerun the relevant generator script, then review the full generated diff.
- The configuration generator uses Python 2-era syntax. `UNKNOWN`: the maintainer-supported Python environment; verify before running it.
- Be alert for duplicate asset basenames: Windows staging flattens directory structure.

## Threading rules

- `resource_accessor` performs CPU serialization on its worker and commit/callback processing through its game-loop update.
- `thread_operation` supports dependency chains split between main and background queues; main work executes from `thread_operations_pool::update()` in the game loop.
- Audio and networking own additional worker/I/O threads.
- Do not capture raw entity/component pointers across queued operations. Prefer `weak_ptr` and validate on execution.
- Do not alter singleton shutdown or queue synchronization without a focused lifecycle test.

## Build and verification

No engine-owned automated test suite was found. Every change still requires the narrowest available compile/content check plus an appropriate demo smoke run.

Windows demo build (evidenced helper):

```powershell
./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64
```

The helper builds, in order: `gbCore`, `gbSound`, `gbDatabase`, `gbUI`, `gbNetwork`, and `gbDemo`; it requires Visual Studio/MSBuild and a machine-level Vulkan SDK, copies `shaderc_shared.dll`, and stages assets.

Before claiming completion:

1. Run `git diff --check`.
2. Build every directly affected Windows project/configuration that is available.
3. For shader/material/pass changes, validate asset filenames and run the demo far enough to execute the affected pass.
4. For platform/backend-specific work, build that target. Do not claim Apple/OpenGL verification from a Windows Vulkan build.
5. Report anything not run and why.

`gbCore/gbCore.sln` references a missing `gbAnimation` project, so prefer the evidenced project-order helper for the Windows demo. `UNKNOWN`: whether current `gbServer` projects build without repairing stale references.

## Adding files

The repository does not use CMake. When adding a C++/Objective-C++ file:

- Add it to the owning Visual Studio `.vcxproj` for Windows if applicable.
- Add it to the owning Xcode target(s) for macOS/iOS/tvOS if applicable.
- Keep platform-specific Objective-C syntax in Objective-C++-compiled files/project entries.
- Verify include paths and static-library dependency order in both build systems.

Do not infer that a file is built merely because it exists under `sources/`.

## Naming and search

Preserve existing public spellings unless the task is an explicit rename. The repository contains legacy names such as `game_conroller`, `commiter`, `transfering`, `compoment`, and filenames such as `ui_editor_scene..cpp`. Search for both the conventional spelling and known legacy spelling before concluding code is absent.

Use `rg`/`rg --files` for repository searches. Exclude `gbCore/libraries`, `gbDemo/Pods`, and build `output` directories unless the task concerns dependencies or generated artifacts.

## High-risk files and areas

Normally avoid modifying these for a local feature:

- `gbCore/sources/main_headers.h` (`STTI_CLASS_GUID`, platform-wide includes/constants).
- `gbCore/sources/ces_entity.*` and `ces_systems_feeder.*` (ECS storage/membership/execution contracts).
- `gbCore/sources/gl_commands.*` (backend selection and OpenGL-shaped compatibility layer).
- `graphics_context_*`, `vk_*`, `mtl_device*` presentation/synchronization code.
- `resource_accessor.*` thread/commit behavior.
- `gbCore/autogenerated/*` and `gbDemo/autogenerated/*`.
- Vendored third-party sources.

If a requested change truly requires one of these, isolate it, explain the invariant being changed, and verify all consumers/backends in scope.

## Known unknowns

Do not silently assume answers to these:

- `UNKNOWN`: current green Apple/Xcode target matrix.
- `UNKNOWN`: intended supported status of dormant OpenGL paths.
- `UNKNOWN`: exact `__NO_RENDER__` to `NO_GRAPHICS_API` mapping for headless server builds.
- `UNKNOWN`: complete Vulkan swapchain resize/device-loss strategy.
- `UNKNOWN`: serialized format compatibility/version policy for GB3D assets.
- `UNKNOWN`: current server build health and missing `gbAnimation` history.

Reference `ARCHITECTURE_ISSUES.md` and investigate the named source/build files when a task touches one of these areas.
