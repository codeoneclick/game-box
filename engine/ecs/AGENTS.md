# ECS Instructions

These instructions apply to entity, component, system, query, registration, and feeder work. Actual ECS source is under `gbCore/sources`; game-specific ECS code belongs under `gbDemo/sources` or `gb_client_server_shared/sources`.

## Where code belongs

- Reusable component/system/entity preset: `gbCore/sources`.
- Racing-game-only state or behavior: `gbDemo/sources`.
- Behavior intentionally shared by client and server: `gb_client_server_shared/sources`.
- Scene-specific system registration: the owning scene's `create()` implementation.
- Systems required by every transition: consider `gbCore/sources/game_transition.cpp`, but modify this only when the requirement is truly global.

Choose ownership from existing consumers. A generic name in a short prompt does not automatically make the feature engine-core functionality.

## Canonical patterns to copy

- Entity construction: `gbCore/sources/scene_fabricator.cpp::create_shape_3d()` and `gbCore/sources/ces_entity.h`.
- Component declaration: `gbDemo/sources/ces_car_input_component.h` and `.cpp`.
- System declaration/query: `gbDemo/sources/ces_car_boost_system.h` and `.cpp`.
- Core system ordering/registration: `gbCore/sources/game_transition.cpp`.
- Application system registration: `gbDemo/sources/in_game_scene.cpp` and `main_menu_scene.cpp`.

Use the complete examples and explanation in `CODE_PATTERNS.md`; do not copy an isolated macro or callback without its construction and registration steps.

## Adding a component

For a prompt such as “Add a HealthComponent”:

1. Find the closest data/ownership analogue in the owning module.
2. Derive from `gb::ces_base_component`.
3. Declare `STTI_CLASS_GUID(ComponentType, gb::ces_base_component::g_guids_container)` with qualification appropriate to the namespace.
4. Keep state in the component; expose it using ordinary methods or the established `std::property_*` convention used by the analogue.
5. Attach with `entity->add_component<ComponentType>()`, normally from an entity preset or fabricator.
6. Add new `.h`/`.cpp` files to applicable Visual Studio and Xcode project metadata.

Do not rely on `ces_base_component::m_owner`: `set_owner()` has no definition or call site. Pass/query the entity explicitly when behavior needs it.

## Adding a system

For a prompt such as “Create a rotation system”:

1. Derive from `gb::ces_base_system` and add the system STTI declaration.
2. Store each required mask as `std::bitset<std::numeric_limits<uint8_t>::max()>`.
3. In the constructor, call `add_required_component_guid()` for every required component, then `add_required_components_mask()` once for that mask.
4. Implement `on_feed_start()`, `on_feed()`, and `on_feed_end()`; empty overrides are required when a phase is unused.
5. Use `enumerate_entities_with_components()` in `on_feed()`. Do not recursively walk the scene tree when a component query expresses the requirement.
6. Register explicitly with `game_transition::add_system()` from the appropriate transition/scene.
7. Inspect every registration in that transition and assign an intentional `ui8` order. The default is 255; equal values are permitted but obscure dependencies.

## Lifecycle and registration rules

- Component and system type IDs are allocated lazily on first `class_guid()` use. They are process-order-dependent and are not serialization IDs.
- The component array has 255 slots, but IDs begin at 1; only IDs 1 through 254 are safely addressable and there is no overflow guard.
- Adding/removing a component or attaching/detaching an entity queues a feeder change. Query membership is repaired at the next feeder update, not immediately.
- A frame executes all systems' `on_feed_start`, then all `on_feed`, then all `on_feed_end`, followed by cleanup passes. Do not assume one system completes all three phases before the next begins.
- Query lists hold weak entity references. Entities strongly own components and children; children weakly reference parents and feeders.
- `ces_entity::add_child()` is not proven to safely reparent an already-attached child. Detach deliberately before reparenting.
- `set_is_paused()` writes a field that feeder eligibility does not read. Use/investigate `ces_system_modifiers_component` rather than assuming `set_is_paused()` pauses execution.
- `ces_systems_feeder::add_system()` has no duplicate guard for its ordered list. Do not register the same system type twice.

## What not to touch

For an ordinary component or system, do not modify:

- `gbCore/sources/main_headers.h` or `STTI_CLASS_GUID`
- `gbCore/sources/ces_entity.*`
- `gbCore/sources/ces_base_component.*`
- `gbCore/sources/ces_base_system.*`
- `gbCore/sources/ces_systems_feeder.*`
- `gbCore/sources/game_loop.*`

Do not instantiate a derived entity with `std::make_shared<T>()` when it expects deferred/default components; use `ces_entity::construct<T>()`.

## Build and test

There is no discovered engine-owned ECS test target. For an ECS change:

1. Compile every owning module whose project metadata changed.
2. Run the standard Windows demo build when available:

   ```powershell
   ./gbScripts/build-gbDemo.ps1 -Configuration debug-client -Platform x64
   ```

3. Smoke-test the scene that registers the system or creates the entity.
4. Exercise entity creation, component presence/default values, first-frame query discovery, update behavior, and removal/detachment when applicable.
5. Verify explicit system order against neighboring systems.
6. Report Apple/server targets not built; do not infer them from the Windows demo.

