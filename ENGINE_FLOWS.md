# Game Box Engine Runtime Flows

All names and ordering below are derived from current call sites. Backend branches are compile-time choices made through `USED_GRAPHICS_API` in `gbCore/sources/gl_commands.hpp`.

## Application startup

```mermaid
flowchart TD
    A[Platform entry point] --> B[Application delegate]
    B --> C[Platform game-controller wrapper]
    C --> D[window_impl]
    C --> E[game::main_game_controller]
    E --> F[gb::game_controller constructor]
    F --> G[graphics_context::construct]
    F --> H[input_context::construct]
    F --> I[configuration_accessor]
    F --> J[resource_accessor worker + game-loop listener]
    E --> K[queue main-thread thread_operation]
    K --> L[construct application game_transition]
    L --> M[game_controller::goto_transition]
    M --> N[game_transition::on_activated]
    N --> O[parse transition configuration]
    N --> P[construct render pipeline and techniques]
    N --> Q[register built-in systems]
    N --> R[construct scene and fabricator]
    N --> S[attach feeder and call scene::create]
    A --> T[native/game-box run loop]
    T --> U[game_loop::on_update]
```

Windows uses `gbDemo/sources/main.cpp` -> `application_delegate_win32` -> `game_conroller_win32`; Apple uses `gbDemo/sources/main.m` and native app delegates. The Windows controller creates a 1280x720 `window_impl`. `main_game_controller::goto_island_demo_scene()` deliberately queues transition creation through a main-queue `thread_operation`, which is executed by `thread_operations_pool::update()` from the game loop.

Transition activation is the composition root for one running world. The default demo loads `transition.island_demo.xml`, builds render techniques, installs default system orders 0 through 7, registers the feeder with the game loop, constructs the scene, assigns feeder/root, and invokes `scene_graph::create()`.

## Frame update

```mermaid
sequenceDiagram
    participant Native as Native run loop
    participant Loop as game_loop
    participant Resources as resource_accessor
    participant Feeder as ces_systems_feeder
    participant Systems as ordered systems
    participant Jobs as thread_operations_pool

    Native->>Loop: on_update()
    Loop->>Loop: compute dt from steady_clock
    loop every unpaused listener in std::set order
        Loop->>Resources: on_update(dt)
        Resources->>Resources: commit waiting assets / callbacks
        Loop->>Feeder: on_update(dt)
        Feeder->>Feeder: apply queued entity membership changes
        Feeder->>Systems: all on_feed_start(dt)
        Feeder->>Systems: all on_feed(root, dt)
        Feeder->>Systems: all on_feed_end(dt)
        Feeder->>Systems: cleanup(root)
        Feeder->>Systems: cleanup_done(root)
    end
    Loop->>Jobs: update main-queue dependencies
    Loop->>Loop: update FPS counters
```

Important ordering caveat: game-loop listeners are held in `std::set<shared_ptr<i_game_loop>>`, so their order is pointer-owner ordering, not explicit semantic ordering. Within the ECS feeder, systems are explicitly sorted by their `ui8 order`, but execution is grouped by phase.

## Entity creation and scene attachment

```mermaid
flowchart TD
    A[ces_entity::construct Derived args] --> B[std::make_shared Derived]
    B --> C[set m_self_weak]
    C --> D[construct_components]
    D --> E[run deferred component constructors]
    E --> F[add each component and set mask bit]
    F --> G[Derived::setup_components]
    G --> H[caller configures components/tag]
    H --> I[parent::add_child]
    I --> J[set weak parent]
    I --> K[strong-own in unordered + ordered child containers]
    I --> L[recursively assign systems feeder]
    L --> M[queue entity-added event]
    M --> N[next feeder update evaluates every registered mask]
```

Canonical constructor behavior is in `gbCore/sources/ces_entity.h`. Derived entity constructors such as `game_object_3d` queue default transformation/geometry/material-related components using `add_deferred_component_constructor`. Calling `std::make_shared<Derived>` directly bypasses `m_self_weak`, deferred construction, and `setup_components()`.

## Component type registration and mutation

There is no explicit startup registry. Registration is lazy and process-local.

```mermaid
flowchart TD
    A[First T::class_guid call] --> B[std::call_once]
    B --> C[insert address of class_guid into global GUID set]
    C --> D[guid = set.size]
    D --> E[use guid as entity array index + bit position]
    E --> F[entity::add_component]
    F --> G[store shared_ptr in m_components guid]
    F --> H[set m_mask guid]
    F --> I[queue changed event if entity is on scene]
    I --> J[next feeder update removes old query references]
    J --> K[re-add entity to every newly satisfied mask]
```

`STTI_CLASS_GUID` in `main_headers.h` supplies this behavior. Components and systems use separate static GUID sets because they pass `ces_base_component::g_guids_container` or `ces_base_system::g_guids_container`. IDs are not stable across processes or guaranteed across different first-use orders.

## System definition, registration, and update

```mermaid
flowchart LR
    A[System constructor] --> B[add_required_component_guid for each type]
    B --> C[add_required_components_mask]
    C --> D[set_order]
    D --> E[game_transition::add_system]
    E --> F[feeder map by instance_guid]
    E --> G[ordered list sorted by order]
    H[entity changes] --> I[query weak-reference lists repaired]
    I --> J[on_feed_start]
    J --> K[on_feed root]
    K --> L[enumerate_entities_with_components mask]
    L --> M[on_feed_end]
```

Registration examples occur in `game_transition::on_activated()` for core systems and scene `create()` implementations for application systems. A system can register multiple masks and enumerate each independently. `can_be_feeded()` also consults the root `ces_system_modifiers_component`, permitting per-system pause; the system's own `m_is_paused` flag exists but is not consulted by `ces_systems_feeder` in the inspected path.

## Mesh loading

```mermaid
sequenceDiagram
    participant Caller as scene_fabricator/caller
    participant RA as resource_accessor
    participant Op as mesh_3d_loading_operation
    participant Ser as GB3DMESH or FBX serializer
    participant Commit as mesh_3d_commiter_gbmesh3d
    participant Mesh as mesh_3d

    Caller->>RA: get_resource<mesh_3d, mesh_3d_loading_operation>(file, sync)
    RA->>RA: return cache hit, or create Mesh + Op
    alt GB3DMESH/GB3D_MESH
        Op->>Ser: mesh_3d_serializer_gbmesh3d::serialize
    else FBX
        Op->>Ser: mesh_3d_serializer_fbx::serialize
    end
    Ser->>Mesh: on_transfering_data_serialized
    Note over Ser,Mesh: CPU vertices/indices/skeleton/bounds become transfer data
    Op->>Commit: commit transfer data
    Commit->>Mesh: create/assign VBO + IBO and committed state
    RA->>Caller: callback/listener on game loop
```

FBX support is routed through `mesh_3d_serializer_fbx.cpp` (ufbx) and then the same GB mesh committer. `scene_fabricator::create_shape_3d()` currently requests meshes synchronously (`sync=true`). File extension checks are string searches and partly case-specific.

## Texture loading

```mermaid
sequenceDiagram
    participant Material as material::set_textures
    participant RA as resource_accessor
    participant Op as texture_loading_operation
    participant Ser as texture_serializer_png
    participant Commit as texture_commiter_png
    participant GPU as selected backend

    Material->>RA: get_resource<texture, texture_loading_operation>(png, force)
    RA->>Op: queue or run synchronously
    Op->>Ser: decode PNG to transfer data
    Ser-->>Op: loaded pixels, dimensions, format
    Op->>Commit: commit on game-loop/main path
    Commit->>GPU: create/upload texture
    Commit-->>Material: texture resource becomes committed
```

The loading operation contains an empty `.pvr` branch and asserts for other formats; PNG is the evidenced 2D texture path. Cubemaps have parallel `cubemap_texture_*_png` classes. Render-pass attachment textures bypass file loading and are registered with `resource_accessor::add_custom_resource()`.

## Shader creation

```mermaid
flowchart TD
    A[Material XML shader filename base] --> B[material::set_shader]
    B --> C[resource_accessor get shader]
    C --> D[shader_loading_operation]
    D --> E[append .vert and .frag]
    E --> F[shader_serializer_glsl reads sources]
    F --> G[shader_commiter_glsl]
    G --> H{Backend}
    H -->|OpenGL| I[compile/link GLSL program]
    H -->|Vulkan| J[shaderc/backend shader modules and descriptors]
    H -->|Metal| K[resolve functions from default Metal library]
    I --> L[shader committed]
    J --> L
    K --> L
```

The base-name convention is visible in `shader_loading_operation.cpp`. Vulkan compilation and Metal symbol mapping contain backend-specific details in the shader/material implementation; adding only one GLSL file is insufficient because the loader always requests both stages.

## Material creation and attachment

```mermaid
flowchart TD
    A[configuration_accessor loads material XML] --> B[material_configuration]
    B --> C[material::construct copies fixed state]
    C --> D[material::set_shader]
    C --> E[material::set_textures]
    D --> F[resource cache/loading]
    E --> F
    F --> G[ces_material_component::add_material]
    G --> H[technique name receives compact technique index]
    H --> I[render system retrieves material by pass index]
```

`scene_fabricator::add_materials()` is the normal integration point and forces synchronous resource loading. Transition screen-space/output materials use the same construction calls directly in `game_transition.cpp`.

## Render submission

```mermaid
sequenceDiagram
    participant Feeder as ces_systems_feeder
    participant Render as ces_render_system
    participant Pipeline as render_pipeline
    participant Pass as render_technique_ws
    participant Material as material/component
    participant Mesh as mesh

    Feeder->>Render: on_feed_start(dt)
    Render->>Pipeline: on_draw_begin()
    Feeder->>Render: on_feed(root, dt)
    Render->>Render: propagate root SS/output uniforms
    Feeder->>Render: on_feed_end(dt)
    loop ordered WS techniques
        Render->>Pass: bind()
        Render->>Render: query render mask and cull by camera/frustum
        loop visible 2D then 3D entities
            Render->>Material: on_bind(technique, vertex input)
            Render->>Material: set P/V/M/N and custom uniforms
            Render->>Mesh: bind(), draw(), unbind()
            Render->>Material: on_unbind()
        end
        Render->>Pass: unbind()
    end
    Render->>Pipeline: on_draw_end()
```

Renderable membership requires transformation, geometry, and material components. Visibility also requires both `visible` and `visible_in_next_frame`. 2D entities are ordered in `std::map<f32,...>` by z order; 3D entities use queues in the observed non-batching path. Technique selection is data-driven by material `technique_name` and pass GUID naming conventions.

## Render-pass execution

```mermaid
flowchart TD
    A[Transition XML] --> B[ordered WS configurations]
    A --> C[ordered SS configurations]
    A --> D[output configuration]
    B --> E[WS: geometry passes into attachments]
    E --> F[attachment textures registered as resources]
    C --> G[SS: fullscreen quad materials sample earlier attachments]
    G --> H[skip-frame policy]
    D --> I[Main: fullscreen quad into drawable/swapchain]
    F --> G
    H --> I
    I --> J[presentation]
```

WS pass order comes from `ws_technique_configuration::order` and the importer. SS passes are drawn by `render_pipeline::on_draw_end()` in their configured ordered list. `render_technique_ss` and `render_technique_main` draw screen quads. OpenGL constructs FBOs directly; Vulkan and Metal use the common `render_pass_descriptor` facade.

## GPU presentation

```mermaid
flowchart TD
    A[render_pipeline::on_draw_end] --> B{Backend}
    B -->|OpenGL| C[technique main unbind]
    C --> D[graphics_context::draw]
    D --> E[SwapBuffers / CGLFlushDrawable / presentRenderbuffer]
    B -->|Vulkan| F[vkEndCommandBuffer]
    F --> G[graphics_context_win32::draw]
    G --> H[vkQueueSubmit with image-available/render-finished semaphores]
    H --> I[vk_swap_chain::queue_present]
    I --> J[advance frame fence slot]
    B -->|Metal| K[graphics_context::draw]
    K --> L[mtl_device::unbind]
    L --> M[commit WS and SS command buffers]
    M --> N[present drawable on output command buffer]
    N --> O[commit output command buffer]
```

For Vulkan, image acquisition happens earlier in `graphics_context_win32::make_current()`. For Metal, three command buffers are created in `mtl_device` frame bind and committed in a fixed WS -> SS -> output order, guarded by dispatch semaphores.

## Configured scene/level loading

The term “scene” has two paths in this repository:

1. Runtime application scenes (`scene_graph` subclasses) are created by transitions and usually assemble entities procedurally through fabricators.
2. Asset scenes (`scene_2d` TMX and `scene_3d` GB3DSCENE resources) are loaded through the resource pipeline and consumed by fabricators.

```mermaid
sequenceDiagram
    participant Transition as application transition
    participant Runtime as scene_graph subclass
    participant Fabricator as gameplay/scene fabricator
    participant RA as resource_accessor
    participant SceneAsset as scene_2d or scene_3d
    participant ECS as runtime entities

    Transition->>Runtime: ces_entity::construct<Scene>(transition)
    Transition->>Runtime: set fabricator
    Transition->>Runtime: create()
    Runtime->>Fabricator: create_level / create_shape / create_sprite
    Fabricator->>RA: get scene asset (often sync=true)
    RA->>SceneAsset: serialize then commit
    SceneAsset-->>Fabricator: layers/objects/points or 3D object data
    Fabricator->>ECS: construct entities + components
    Fabricator->>ECS: add route/physics/material/mesh data
    Runtime->>ECS: attach children to scene root
```

`gameplay_fabricator::create_level()` is the strongest current 2D scene-loading example: it loads a TMX `scene_2d`, interprets named object groups such as `route`, creates 3D track/building/gameplay entities, and builds Box2D data. This is application-specific interpretation, not a generic automatic TMX-to-ECS world instantiator.

## Transition change and shutdown

```mermaid
flowchart TD
    A[game_controller::goto_transition new GUID] --> B{current transition exists?}
    B -->|yes| C[current::on_deactivated]
    C --> D[remove feeder from game loop]
    C --> E[destroy_scene virtual]
    C --> F[feeder cleanup systems/root]
    F --> G[remove old transition from map]
    B -->|no| H[select new transition]
    G --> H
    H --> I[new::on_activated]
```

The `game_controller` destructor clears transitions, and the `resource_accessor` destructor stops/joins its worker. Exact application shutdown ordering across native delegates, static graphics singletons, the job-pool singleton, audio, and network threads is `UNKNOWN`; investigate those destructors before adding shutdown-sensitive work.

## Island ocean frame

```mermaid
flowchart LR
    A[Ocean clock and CPU wave model] --> B[Buoyancy probes and transforms]
    A --> C[Ocean and receiver uniforms]
    B --> D[Opaque ws.base: terrain seabed props]
    C --> D
    D --> E[Scene color normal and position/depth color attachments]
    E --> F[ws.ocean: waves refraction SSR]
    C --> F
    F --> G[ss.ocean_compose]
    D --> G
    G --> H[Output gamma and presentation]
```

Water samples separate scene-data color attachments, never the pooled active
hardware depth attachment. Empty scene position/depth has zero alpha; valid
geometry has positive linear view depth. CPU and shader sampling share a direct static-XZ domain-warped height field.
Geometry mask, seed/JFA/resolve and raised foam are configured WS stages before
composition; their field is independent of the player camera. The ocean system
clamps simulation time to 0.25 s per frame and updates uniforms after buoyancy.
