#include "rhythm_ship_environment.h"
#include "ces_geometry_3d_component.h"
#include "ces_material_component.h"
#include "ces_ocean_system.h"
#include "ces_render_system.h"
#include "ces_shader_uniforms_component.h"
#include "ces_transformation_3d_component.h"
#include "configuration_accessor.h"
#include "game_transition.h"
#include "glm_extensions.h"
#include "ibo.h"
#include "material.h"
#include "material_configuration.h"
#include "mesh_3d.h"
#include "ocean_mesh_builder.h"
#include "render_pipeline.h"
#include "render_technique_ws.h"
#include "scene_fabricator.h"
#include "shape_3d.h"
#include "shape_3d_configuration.h"
#include "texture.h"
#include "vbo.h"
#include <cmath>
namespace game
{
namespace
{
class rhythm_glow_uniforms final : public gb::ces_shader_uniforms_component::shader_uniforms
{
    glm::vec4 value_ = glm::vec4(0);

  public:
    rhythm_glow_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode mode,
                         const std::string &name)
        : shader_uniforms(mode, name)
    {
        m_uniforms["stage_glow"] = std::make_shared<gb::shader_uniform>(gb::e_uniform_type_vec4);
        set(value_, "stage_glow");
    }
    void *get_values() override
    {
        value_ = m_uniforms["stage_glow"]->get_vec4();
        return &value_;
    }
    ui32 get_values_size() override
    {
        return sizeof(value_);
    }
};
} // namespace
gb::mesh_3d_shared_ptr rhythm_upload_mesh(const terrain_mesh_data &data, const std::string &id)
{
    auto decl = std::make_shared<gb::vbo::vertex_declaration_PTNTC>(
        static_cast<ui32>(data.vertices.size()));
    auto vb = std::make_shared<gb::vbo>(decl, gb::gl::constant::static_draw);
    auto p = vb->lock<gb::vbo::vertex_attribute_PTNTC>();
    glm::vec3 lo(1e20f), hi(-1e20f);
    for (const auto &v : data.vertices)
    {
        *p = {};
        p->m_position = glm::vec3(v.position[0], v.position[1], v.position[2]);
        p->m_normal = glm::packSnorm4x8(glm::vec4(v.normal[0], v.normal[1], v.normal[2], 0));
        p->m_texcoord = glm::packHalf2x16(glm::vec2(v.texcoord[0], v.texcoord[1]));
        p->m_color = glm::u8vec4(v.color[0], v.color[1], v.color[2], v.color[3]);
        lo = glm::min(lo, p->m_position);
        hi = glm::max(hi, p->m_position);
        ++p;
    }
    vb->unlock();
    auto ib = std::make_shared<gb::ibo>(static_cast<ui32>(data.indices.size()),
                                        gb::gl::constant::static_draw);
    std::copy(data.indices.begin(), data.indices.end(), ib->lock());
    ib->unlock();
    return gb::mesh_3d::construct(id, vb, ib, hi + glm::vec3(1), lo - glm::vec3(1));
}
gb::shape_3d_shared_ptr rhythm_shape(const gb::scene_fabricator_shared_ptr &fab,
                                     const gb::ces_entity_shared_ptr &root,
                                     const terrain_mesh_data &mesh, const std::string &id,
                                     const std::string &config)
{
    auto s = gb::ces_entity::construct<gb::shape_3d>();
    s->tag = id;
    s->get_component<gb::ces_geometry_3d_component>()->set_mesh(rhythm_upload_mesh(mesh, id));
    s->get_component<gb::ces_geometry_3d_component>()->bounding_mode =
        gb::ces_geometry_component::e_none;
    auto c = std::static_pointer_cast<gb::shape_3d_configuration>(
        fab->get_configuration_accessor()->get_shape_3d_configuration(config));
    fab->add_materials(s, c->get_materials_configurations());
    if (config == "rhythm_ship_solid.xml")
        s->add_component<gb::ces_shader_uniforms_component>()
            ->construct_uniforms_buffer<rhythm_glow_uniforms>(
                gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment, "rhythm.glow",
                0);
    root->add_child(s);
    return s;
}
namespace
{
void box(terrain_mesh_data &m, std::array<float, 3> point, std::array<float, 3> dimensions,
         std::array<ui8, 4> tint)
{
    glm::vec3 p(point[0], point[1], point[2]), size(dimensions[0], dimensions[1], dimensions[2]);
    glm::u8vec4 color(tint[0], tint[1], tint[2], tint[3]);
    const glm::vec3 n[6] = {glm::vec3(1, 0, 0),  glm::vec3(-1, 0, 0), glm::vec3(0, 1, 0),
                            glm::vec3(0, -1, 0), glm::vec3(0, 0, 1),  glm::vec3(0, 0, -1)};
    for (auto normal : n)
    {
        glm::vec3 u = std::abs(normal.y) > .5f ? glm::vec3(1, 0, 0)
                                               : glm::vec3(normal.z, 0, -normal.x),
                  v = glm::cross(normal, u);
        ui16 base = static_cast<ui16>(m.vertices.size());
        for (int k = 0; k < 4; ++k)
        {
            glm::vec3 pos =
                p + (normal + u * (k % 2 ? 1.f : -1.f) + v * (k / 2 ? 1.f : -1.f)) * size * .5f;
            m.vertices.push_back({{{pos.x, pos.y, pos.z}},
                                  {{normal.x, normal.y, normal.z}},
                                  {{color.r, color.g, color.b, color.a}}});
        }
        for (int i : {0, 1, 2, 1, 3, 2})
            m.indices.push_back(base + i);
    }
}
terrain_mesh_data quad()
{
    terrain_mesh_data m;
    for (int k = 0; k < 4; ++k)
        m.vertices.push_back(
            {{{k % 2 ? 1.f : -1.f, k / 2 ? 1.f : -1.f, 0}}, {{0, 0, 1}}, {{255, 255, 255, 255}}});
    m.indices = {0, 1, 2, 1, 3, 2};
    return m;
}
} // namespace
RhythmShipEnvironment create_rhythm_ship_environment(
    const gb::scene_fabricator_shared_ptr &fab, const gb::ces_entity_shared_ptr &root,
    const gb::game_transition_shared_ptr &transition)
{
    RhythmShipEnvironment e;
    ocean_settings settings;
    terrain_config config;
    // The legacy renderer assumes every configured WS technique has a registered
    // material index, even when that pass has no geometry in this scene. Keep one
    // hidden registry entity so empty grass/debug/shadow passes can clear safely.
    auto registry = gb::ces_entity::construct<gb::shape_3d>();
    registry->tag = "rhythm.empty_pass_registry";
    registry->visible = false;
    registry->visible_in_next_frame = false;
    for (const auto &pass : transition->get_system<gb::ces_render_system>()
                                ->get_render_pipeline()
                                ->get_ws_techniques())
        registry->get_component<gb::ces_material_component>()->add_material(
            pass->get_name().substr(pass->get_name().find('_') + 1),
            std::make_shared<gb::material>());
    root->add_child(registry);
    for (auto &technique : transition->get_system<gb::ces_render_system>()
                               ->get_render_pipeline()
                               ->get_ws_techniques())
        if (technique->get_name().find("ws.water.distance") != std::string::npos)
        {
            auto t = technique->get_color_attachment_texture();
            if (t)
                settings.interaction_resolution = float(t->get_width());
        }
    e.ocean = std::make_shared<ces_ocean_system>(settings);
    e.ocean->set_world_dimensions(config.world_size, config.water_size);
    transition->add_system(e.ocean);
    auto sky = rhythm_shape(fab, root, quad(), "rhythm.sky", "island_sky.xml");
    e.ocean->add_receiver(sky);
    for (const auto &name : {"water_seed", "water_jfa0", "water_jfa1", "water_jfa2", "water_jfa3",
                             "water_jfa4", "water_jfa5", "water_jfa6", "water_jfa7", "water_jfa8",
                             "water_jfa9", "water_jfa10", "water_jfa11", "water_distance"})
    {
        auto q = rhythm_shape(fab, root, quad(), std::string("rhythm.field.") + name,
                              std::string(name) + ".xml");
        if (std::string(name) == "water_distance")
            e.ocean->add_receiver(q);
    }
    auto water_config = std::static_pointer_cast<gb::shape_3d_configuration>(
        fab->get_configuration_accessor()->get_shape_3d_configuration("ocean.xml"));
    auto add_water = [&](const terrain_mesh_data &data, const std::string &id, bool horizon) {
        auto water = gb::ces_entity::construct<gb::shape_3d>();
        water->tag = id;
        water->get_component<gb::ces_geometry_3d_component>()->set_mesh(
            rhythm_upload_mesh(data, id));
        water->get_component<gb::ces_geometry_3d_component>()->bounding_mode =
            gb::ces_geometry_component::e_none;
        std::vector<std::shared_ptr<gb::configuration>> materials, foam_materials;
        for (auto c : water_config->get_materials_configurations())
            if (std::static_pointer_cast<gb::material_configuration>(c)->get_technique_name() ==
                "ws.ocean.foam")
                foam_materials.push_back(c);
            else
                materials.push_back(c);
        fab->add_materials(water, materials);
        root->add_child(water);
        gb::shape_3d_shared_ptr foam;
        if (!horizon)
        {
            foam = gb::ces_entity::construct<gb::shape_3d>();
            fab->add_materials(foam, foam_materials);
            root->add_child(foam);
        }
        e.ocean->add_surface(water, foam, data, horizon);
    };
    auto patches = ocean_mesh_builder::build(config.water_size, 256);
    for (size_t i = 0; i < patches.size(); ++i)
        add_water(patches[i], "rhythm.water." + std::to_string(i), false);
    add_water(ocean_mesh_builder::build_horizon(config.water_size, 200000.f, 256), "rhythm.horizon",
              true);
    terrain_mesh_data hull;
    box(hull, {0, 1, 0}, {39, 4, 85}, {225, 231, 239, 0});
    auto hull_entity = rhythm_shape(fab, root, hull, "rhythm.hull", "rhythm_ship_solid.xml");
    auto mask = rhythm_shape(fab, root, hull, "rhythm.hull.mask", "water_mask.xml");
    e.ocean->add_interaction(hull_entity, mask);
    terrain_mesh_data deck;
    box(deck, {0, 5.6f, -29}, {3, 1.6f, 3}, {38, 39, 62, 0});
    box(deck, {0, 3.15f, 0}, {40, .4f, 85}, {206, 165, 112, 0});
    box(deck, {0, 3.4f, 1}, {28, .2f, 34}, {24, 30, 54, 0});
    box(deck, {0, 4, -25}, {32, 1.6f, 14}, {38, 39, 62, 0});
    box(deck, {0, 6.3f, -28}, {8, 3, 2}, {18, 22, 39, 0});
    box(deck, {0, 6, -38}, {35, 6, 8}, {231, 237, 241, 0});
    box(deck, {0, 9.3f, -39}, {29, .6f, 6}, {32, 59, 80, 0});
    for (int z = -38; z <= 39; z += 5)
    {
        box(deck, {-19.6f, 4, z * 1.f}, {.14f, 1.5f, .14f}, {233, 245, 250, 0});
        box(deck, {19.6f, 4, z * 1.f}, {.14f, 1.5f, .14f}, {233, 245, 250, 0});
    }
    for (float x : {-19.6f, 19.6f})
        for (float y : {3.9f, 4.65f})
            box(deck, {x, y, 0}, {.12f, .12f, 81}, {225, 245, 248, 0});
    for (int i = 0; i < 4; ++i)
        box(deck, {0, 3.55f + i * .35f, -17.5f - i * .7f}, {10, .35f + i * .7f, .75f},
            {166, 180, 202, 0});
    // Truss, canopy and evenly spaced deck seams give the primitive liner a readable silhouette.
    for (float x : {-16.f, 16.f})
        box(deck, {x, 9, -27}, {.45f, 10, .45f}, {42, 51, 65, 0});
    box(deck, {0, 13.5f, -27}, {33, .5f, .6f}, {42, 51, 65, 0});
    for (int z = -40; z < 42; z += 2)
        box(deck, {0, 3.36f, float(z)}, {39, .015f, .035f}, {112, 84, 61, 0});
    for (int x = -12; x <= 12; x += 6)
        box(deck, {x * 1.f, 6, -33.85f}, {3, 1.1f, .1f}, {38, 103, 139, 255});
    rhythm_shape(fab, root, deck, "rhythm.deck", "rhythm_ship_solid.xml");
    for (float x : {-13.f, 13.f})
    {
        terrain_mesh_data m;
        box(m, {0, 0, 0}, {3, 7, 3}, {14, 17, 26, 0});
        auto s = rhythm_shape(fab, root, m, "rhythm.speaker", "rhythm_ship_solid.xml");
        s->position = glm::vec3(x, 7.5, -25);
        e.speakers.push_back(s);
        for (float y : {5.5f, 8.f})
        {
            terrain_mesh_data cone;
            box(cone, {0, 0, 0}, {2, .1f, 2}, {72, 217, 244, 255});
            auto p = rhythm_shape(fab, root, cone, "rhythm.speaker.light", "rhythm_ship_solid.xml");
            p->position = glm::vec3(x, y, -23.4f);
            p->rotation = glm::vec3(1.5708f, 0, 0);
            e.panels.push_back(p);
        }
    }
    for (int i = 0; i < 9; ++i)
    {
        terrain_mesh_data m;
        glm::u8vec4 c = i % 3 == 0   ? glm::u8vec4(225, 48, 180, 255)
                        : i % 3 == 1 ? glm::u8vec4(39, 216, 244, 255)
                                     : glm::u8vec4(178, 113, 255, 255);
        box(m, {0, 0, 0}, {2.4f, 4, .15f}, {c.r, c.g, c.b, c.a});
        auto s = rhythm_shape(fab, root, m, "rhythm.panel", "rhythm_ship_solid.xml");
        s->position = glm::vec3((i - 4) * 3.f, 8, -32);
        e.panels.push_back(s);
    }
    for (int i = 0; i < 8; ++i)
    {
        terrain_mesh_data m;
        box(m, {0, 0, 0}, {.18f, .12f, 13},
            {i % 2 ? ui8(244) : ui8(42), ui8(90), ui8(235), ui8(255)});
        auto s = rhythm_shape(fab, root, m, "rhythm.strip", "rhythm_ship_solid.xml");
        s->position = glm::vec3((i - 3.5f) * 4, 3.53f, 0);
        e.strips.push_back(s);
    }
    terrain_mesh_data ring;
    for (int i = 0; i < 40; ++i)
    {
        float a = i * 6.2831853f / 40, b = (i + 1) * 6.2831853f / 40;
        ui16 base = static_cast<ui16>(ring.vertices.size());
        for (int j = 0; j < 4; ++j)
        {
            float angle = j / 2 ? b : a, r = j % 2 ? 1.6f : 1.3f;
            ring.vertices.push_back({{{std::cos(angle) * r, 0, std::sin(angle) * r}},
                                     {{0, 1, 0}},
                                     {{255, 210, 51, 255}}});
        }
        for (int k : {0, 1, 2, 1, 3, 2})
            ring.indices.push_back(base + k);
    }
    e.ring = rhythm_shape(fab, root, ring, "rhythm.you.ring", "rhythm_ship_solid.xml");
    e.ring->position = glm::vec3(0, 3.57f, 9);
    for (size_t i = 0; i < e.floor_pulses.size(); ++i)
    {
        auto pulse_mesh = ring;
        for (auto &v : pulse_mesh.vertices)
            v.color = {{38, 210, 255, 255}};
        e.floor_pulses[i] =
            rhythm_shape(fab, root, pulse_mesh, "rhythm.floor.pulse." + std::to_string(i),
                         "rhythm_ship_solid.xml");
        e.floor_pulses[i]->position = glm::vec3(0, 3.59f, -1);
        e.floor_pulses[i]->visible = false;
        e.floor_pulses[i]->visible_in_next_frame = false;
    }
    terrain_mesh_data crown;
    box(crown, {0, 0, 0}, {1.5f, .25f, .3f}, {255, 209, 45, 255});
    for (int i = -1; i <= 1; ++i)
        box(crown, {i * .55f, .35f, 0}, {.25f, .6f, .3f}, {255, 223, 75, 255});
    e.crown = rhythm_shape(fab, root, crown, "rhythm.crown", "rhythm_ship_solid.xml");
    e.crown->position = glm::vec3(0, 7.5f, 9);
    e.crown->visible = false;
    return e;
}
void update_rhythm_ship_effects(RhythmShipEnvironment &e, double t, double last, double energy,
                                float intensity, bool leader, const RhythmStageFrame &frame)
{
    float p = static_cast<float>(std::exp(-std::max(0., t - last) * 9));
    for (auto s : e.speakers)
        s->scale = glm::vec3(1 + (.05f * p + .08f * frame.speaker) * intensity);
    for (size_t i = 0; i < e.panels.size(); ++i)
    {
        e.panels[i]->scale =
            glm::vec3(1, (.65f + (.25f + energy * .5f) * p * intensity) * frame.vertical, 1);
        auto glow = e.panels[i]->get_component<gb::ces_shader_uniforms_component>()->get_uniforms(
            gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment, "rhythm.glow", 0);
        glow->set(glm::vec4(intensity * (.1f + float(energy) * .6f + p * .4f +
                                         (energy > .9 && p > .9 ? .7f : 0.f)),
                            0, 0, 0),
                  "stage_glow");
    }
    for (size_t i = 0; i < e.strips.size(); ++i)
    {
        e.strips[i]->rotation = glm::vec3(0, .1f * std::sin(float(t) * .8f + i), 0);
        const bool active = i < 4 || (energy > .25 && i < 6) || energy > .5;
        e.strips[i]->visible = active;
        e.strips[i]->visible_in_next_frame = active;
        e.strips[i]
            ->get_component<gb::ces_shader_uniforms_component>()
            ->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,
                           "rhythm.glow", 0)
            ->set(glm::vec4(frame.glow * (1 + .35f * frame.sweep * (i < 4 ? -1.f : 1.f)), 0, 0, 0),
                  "stage_glow");
        e.strips[i]->rotation =
            glm::vec3(0, .1f * std::sin(float(t) * .8f + i) + frame.sweep * .3f, 0);
    }
    for (size_t i = 0; i < e.floor_pulses.size(); ++i)
    {
        const auto pulse = e.pulses.sample(i, t);
        auto shape = e.floor_pulses[i];
        shape->visible = pulse.active;
        shape->visible_in_next_frame = pulse.active;
        if (pulse.active)
        {
            shape->scale =
                glm::vec3(pulse.scale * frame.floor_scale, 1, pulse.scale * frame.floor_scale);
            shape->get_component<gb::ces_shader_uniforms_component>()
                ->get_uniforms(gb::ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment,
                               "rhythm.glow", 0)
                ->set(glm::vec4(frame.glow * pulse.opacity, 0, 0, 0), "stage_glow");
        }
    }
    e.ring->scale = glm::vec3(1 + (leader ? .2f : .07f) * p);
    e.crown->visible = leader;
    e.crown->visible_in_next_frame = leader;
    e.crown->rotation = glm::vec3(0, float(t) * .4f, 0);
}
} // namespace game
