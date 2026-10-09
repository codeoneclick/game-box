# Canonical Code Patterns

These patterns favor paths used by the current demo and core composition code. Snippets are intentionally minimal and omit unrelated gameplay setup. They are examples of repository conventions, not newly invented APIs.

## Creating an entity

**Best source example:** `gbCore/sources/scene_fabricator.cpp` (for example `create_shape_3d()`); construction contract: `gbCore/sources/ces_entity.h`.

```cpp
auto object = gb::ces_entity::construct<gb::game_object_3d>();
object->tag = "track_piece";
scene->add_child(object);
```

Use `ces_entity::construct<T>()`, not `std::make_shared<T>()`. The factory sets the entity's self weak pointer, materializes components queued by the derived constructor, and calls `setup_components()`. Attaching to the scene makes the feeder discover the entity on the next ECS update.

## Defining a component

**Best source example:** `gbDemo/sources/ces_car_input_component.h` and `.cpp`.

```cpp
class ces_example_component : public gb::ces_base_component
{
private:
    f32 m_value = 0.f;

public:
    STTI_CLASS_GUID(ces_example_component,
                    gb::ces_base_component::g_guids_container)

    ces_example_component()
    {
        value.getter([=]() { return m_value; });
        value.setter([=](f32 v) { m_value = v; });
    }

    std::property_rw<f32> value;
};
```

This is canonical because it follows the engine's runtime type-ID macro and property wrapper convention while keeping behavior/data local. Many components use ordinary methods rather than `std::property_*`; either is established, but the STTI declaration is required for array/mask lookup.

Attach it with:

```cpp
auto component = entity->add_component<ces_example_component>();
component->value = 1.f;
```

## Defining a system

**Best source example:** `gbDemo/sources/ces_car_boost_system.h` and `.cpp`.

```cpp
class ces_example_system : public gb::ces_base_system
{
private:
    std::bitset<std::numeric_limits<uint8_t>::max()> m_mask;

protected:
    void on_feed_start(f32 dt) override {}

    void on_feed(const gb::ces_entity_shared_ptr& root, f32 dt) override
    {
        enumerate_entities_with_components(
            m_mask,
            [dt](const gb::ces_entity_shared_ptr& entity) {
                auto component = entity->get_component<ces_example_component>();
                component->value = component->value + dt;
            });
    }

    void on_feed_end(f32 dt) override {}

public:
    STTI_CLASS_GUID(ces_example_system,
                    gb::ces_base_system::g_guids_container)

    ces_example_system()
    {
        add_required_component_guid(m_mask,
                                    ces_example_component::class_guid());
        add_required_components_mask(m_mask);
    }
};
```

This is canonical because systems declare query masks once in their constructor, then enumerate the feeder-maintained weak-reference list rather than walking the scene tree. All three feed callbacks are abstract in the base and therefore must be implemented even when empty.

## Registering a system

**Best source example:** `gbCore/sources/game_transition.cpp`.

```cpp
auto system = std::make_shared<ces_example_system>();
system->set_order(8);
get_transition()->add_system(system);
```

Core transition setup assigns explicit orders before `add_system()`. This is safer than relying on the default order (`uint8_t` max) and list stability, as several older demo systems do. Check all existing order values in the transition/scene before choosing a number; equal order values are permitted but make intent less clear.

## Querying components

**Best source example:** `gbDemo/sources/ces_car_boost_system.cpp`.

```cpp
enumerate_entities_with_components(
    m_car_components_mask,
    [](const gb::ces_entity_shared_ptr& entity) {
        auto stats = entity->get_component<ces_car_statistic_component>();
        if (stats->mode == ces_car_statistic_component::e_mode_player)
        {
            // Use entity and its required component.
        }
    });
```

The registered mask guarantees the required components existed when membership was last repaired. For an optional component on an already known entity, use `get_component<T>()` and null-check it. `get_component_raw<T>()` is used in the render hot path but should not be kept beyond the owning entity/component lifetime.

## Rendering a mesh

**Best source example:** `gbCore/sources/scene_fabricator.cpp`, especially `create_shape_3d()`; draw path: `gbCore/sources/ces_render_system.cpp`.

```cpp
auto shape = scene->get_fabricator()->create_shape_3d("tree_01.xml");
shape->position = glm::vec3(0.f, 0.f, 0.f);
scene->add_child(shape);
```

The configuration referenced by `tree_01.xml` supplies the mesh filename and material configurations. The fabricator loads the mesh, assigns it to `ces_geometry_3d_component`, creates materials, and associates each material with a render-technique name. Once the entity is attached, `ces_render_system` discovers it through the transformation+geometry+material mask and performs the actual bind/draw/unbind sequence.

For engine-level procedural geometry, the corresponding existing pattern is:

```cpp
auto object = gb::ces_entity::construct<gb::game_object_3d>();
object->add_component<gb::ces_geometry_3d_component>()
      ->set_mesh(gb::mesh_constructor::create_box());
// A material component/material for a configured technique is also required.
```

Prefer the configured fabricator path for game content because it keeps mesh, shader, textures, and render-state data outside gameplay C++.

## Creating a material

**Best source example:** `gbCore/sources/scene_fabricator.cpp::add_materials()`.

```cpp
auto config = std::static_pointer_cast<gb::material_configuration>(
    configuration_accessor->get_material_configuration(
        "material.arrow.base.xml"));

auto material = gb::material::construct(config);
gb::material::set_shader(material, config, resource_accessor, true);
gb::material::set_textures(material, config, resource_accessor, true);
gb::ces_material_extension::add_material(
    entity, config->get_technique_name(), material);
```

This sequence is canonical because `material::construct()` only copies material state; shader and textures are separate resource-loading steps; and the material must be indexed on the entity by technique. In most game code, call a `scene_fabricator` method that performs this sequence rather than duplicating it.

The matching content example is `gbBundle/Configurations/game_objects/materials/material.arrow.base.xml`.

## Loading a texture

**Best source example:** `gbCore/sources/material.cpp::set_textures()`.

```cpp
auto texture = resource_accessor
    ->get_resource<gb::texture, gb::texture_loading_operation>(
        "arrow.png", true);

texture->set_wrap_mode(gb::gl::constant::clamp_to_edge);
texture->set_mag_filter(gb::gl::constant::linear);
texture->set_min_filter(gb::gl::constant::linear);
material->set_texture(texture, gb::e_shader_sampler_01);
```

`e_shader_sampler_01` is sampler index zero in `gbCore/sources/shader.h`; material XML normally avoids spelling it in C++ by using numeric `sampler_index`. The templated `resource_accessor` call is the canonical cache/loading API. `true` forces synchronous serialize+commit, as the fabricator does; use the default asynchronous mode when the caller can wait for the committed callback/state.

Preferred content-driven form, from `material.arrow.base.xml`:

```xml
<texture filename="arrow.png"
         render_operation_name=""
         sampler_index="0"
         wrap_mode="GL_CLAMP_TO_EDGE"
         mag_filter="GL_LINEAR"
         min_filter="GL_LINEAR"/>
```

## Adding a shader

**Best source example:** `gbBundle/Shaders/shader_arrow.vert`, `shader_arrow.frag`, and `gbBundle/Configurations/game_objects/materials/material.arrow.base.xml`; loader: `gbCore/sources/shader_loading_operation.cpp`.

Create a paired base name:

```text
gbBundle/Shaders/shader_example.vert
gbBundle/Shaders/shader_example.frag
```

Reference it in material configuration without an extension:

```xml
<shader filename="shader_example"/>
```

The loader appends `.vert` and `.frag` unconditionally, which makes this the canonical naming convention. No C++ registration table is needed for an external shader. The shader must obey the attribute/uniform conventions consumed by `shader`, `material`, and the selected backend. For Vulkan, validate the GLSL/SPIR-V path; for Metal, verify that the backend can resolve the expected functions in the default Metal library. That cross-backend mapping is not expressed solely by the XML.

## Adding a render pass

**Best source example:** `gbBundle/Configurations/transitions/transition.main_menu.xml`, `gbBundle/Configurations/techniques/ws_techniques/ws.technique.base.xml`, and `gbCore/sources/game_transition.cpp`.

Define a world-space technique configuration:

```xml
<ws_technique guid="ws.example"
              num_passes="1"
              is_depth_compare_mode_enabled="0"
              order="8"
              frame_width="1024"
              frame_height="768">
  <attachments>
    <attachment name="attachment_01"
                pixel_format="71"
                clear_color_r="0.0"
                clear_color_g="0.0"
                clear_color_b="0.0"
                clear_color_a="1.0"/>
  </attachments>
</ws_technique>
```

Reference it from the transition:

```xml
<ws_techniques>
  <!-- existing techniques -->
  <ws_technique filename="ws.technique.example.xml"/>
</ws_techniques>
```

Use a matching material technique name (the render system derives a short technique name from the configured GUID naming convention):

```xml
<material technique_name="ws.example" ...>
  <shader filename="shader_example"/>
  <!-- textures and at least one blending entry -->
</material>
```

This is canonical because transitions are the data-driven render-graph composition point. `game_transition::on_activated()` constructs and registers every configured WS/SS/output technique and exposes attachment textures as custom resources. A new screen-space pass instead needs an `ss_technique` XML plus its fullscreen material and an entry under `<ss_techniques>`; the final output pass is singular.

Do not add a new C++ render-technique subclass unless the pass cannot be represented by the existing WS geometry or SS fullscreen behavior.

## Creating a scene

**Best source examples:** `gbDemo/sources/island_demo_transition.cpp`, `island_demo_scene.h`, and `island_demo_scene.cpp`.

Scene:

```cpp
class example_scene : public gb::scene_graph
{
public:
    explicit example_scene(const gb::game_transition_shared_ptr& transition)
        : gb::scene_graph(transition) {}

    void create() override
    {
        gb::scene_graph::create();

        auto camera2d = std::make_shared<gb::camera_2d>(1280.f, 720.f);
        set_camera_2d(camera2d);

        auto camera3d = std::make_shared<gb::camera_3d>(
            45.f, 0.1f, 1000.f, glm::ivec4(0, 0, 1280, 720), true);
        set_camera_3d(camera3d);

        auto object = get_fabricator()->create_shape_3d("tree_01.xml");
        add_child(object);
    }
};
```

Transition:

```cpp
void example_transition::create_scene()
{
    m_scene = gb::ces_entity::construct<example_scene>(shared_from_this());
    auto fabricator = std::make_shared<gb::scene_fabricator>();
    set_fabricator(fabricator);
}

void example_transition::destroy_scene()
{
    m_scene = nullptr;
}
```

Controller registration:

```cpp
auto transition = std::make_shared<example_transition>(
    "transition.example.xml", get_window(), false);
add_transition(transition);
goto_transition("transition.example.xml");
```

This three-part pattern is canonical because a runtime scene does not stand alone: the transition owns render configuration and the feeder, while the scene owns entities/cameras and registers scene-specific systems. A corresponding transition XML must provide WS/SS/output technique configuration.

## Loading a serialized scene asset

Although not separately requested, this distinction prevents a common mistake.

**Best source example:** `gbDemo/sources/gameplay_fabricator.cpp`.

```cpp
auto scene_asset = resource_accessor
    ->get_resource<gb::scene_2d, gb::scene_2d_loading_operation>(
        "level_1.tmx", true);

for (const auto& route : scene_asset->get_objects("route"))
{
    // Convert TMX object/polyline data into application ECS components.
}
```

A `scene_2d`/`scene_3d` is a resource containing parsed scene data. It is not the same object as the active `scene_graph` root. The application fabricator decides how parsed objects become runtime entities.

## Procedural ocean meshes and receiver uniforms

`gbDemo/sources/island_demo_scene.cpp` is a configured procedural mesh example.
For PTNTC vertices, compute CPU min/max explicitly and use the established
`mesh_3d::construct(guid, vbo, ibo, max, min)` overload; legacy automatic bounds
expect a different packed vertex layout. Rotating ocean props use radius
culling because the box path only transforms two extrema.

`ocean_shader_uniforms.{h,cpp}` shows a matching 496-byte vertex/fragment
uniform wrapper. `ces_ocean_system` attaches it before first rendering and
updates it at order 6. Use `get_uniforms()` with the known wrapper cast here:
the legacy templated `get_uniforms_as<T>` assignment is incompatible with this
derived wrapper. Keep wave settings, time and receiver parameters together;
do not animate floating transforms with a second independent clock.
