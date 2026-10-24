#include "ces_dance_crowd_system.h"
#include "camera_3d.h"
#include "ces_dance_character_component.h"
#include "ces_geometry_3d_component.h"
#include "ces_transformation_3d_component.h"
#include "configuration_accessor.h"
#include "rhythm_dance_pose.h"
#include "rhythm_ship_environment.h"
#include "scene_fabricator.h"
#include "shape_3d.h"
#include "shape_3d_configuration.h"
#include <cmath>
namespace game
{
ces_dance_crowd_system::ces_dance_crowd_system(std::shared_ptr<RhythmSession> s,
                                               std::shared_ptr<gb::al::RhythmAudioTransport> a)
    : session_(s), audio_(a)
{
    set_order(4);
    add_required_component_guid(mask_, ces_dance_character_component::class_guid());
    add_required_components_mask(mask_);
}
void ces_dance_crowd_system::on_feed(const gb::ces_entity_shared_ptr &, f32)
{
    auto camera = get_current_camera_3d();
    if (!camera)
        return;
    double t = audio_->seconds(), phase = session_->beat_position(t) * 6.283185307;
    float energy = static_cast<float>(session_->score().energy);
    enumerate_entities_with_components(mask_, [&](const gb::ces_entity_shared_ptr &e) {
        auto c = e->get_component<ces_dance_character_component>();
        bool active = c->dj || c->index < count_;
        e->visible = active;
        e->visible_in_next_frame = active;
        if (!active)
            return;
        auto p = c->base_position;
        p.y += std::abs(std::sin(float(phase) + c->phase)) * (c->amplitude + energy * .2f);
        auto tr = e->get_component<gb::ces_transformation_3d_component>();
        auto d = camera->get_position() - p;
        float yaw = std::atan2(d.x, d.z);
        RhythmDancePose pose;
        if (!c->dj && choreography_ && (choreography_enabled_ || c->local_player))
        {
            const auto state = choreography_->dancer_state(c->index, t);
            pose = rhythm_dance_pose(state.action, t - state.start, state.strength);
            p += glm::vec3(pose.x, pose.y, pose.z);
        }
        tr->set_position(p);
        tr->set_scale(glm::vec3(pose.scale_x, pose.scale_y, 1));
        tr->set_rotation(
            glm::vec3(0, glm::degrees(yaw),
                      glm::degrees(pose.lean + std::sin(float(phase * .5) * c->speed + c->phase) *
                                                   (.04f + .035f * energy))));
    });
}
void populate_rhythm_ship_crowd(const gb::scene_fabricator_shared_ptr &fab,
                                const gb::ces_entity_shared_ptr &root)
{
    std::array<gb::mesh_3d_shared_ptr, 8> meshes;
    for (int variant = 0; variant < 8; ++variant)
    {
        terrain_mesh_data m;
        for (int k = 0; k < 4; ++k)
        {
            terrain_vertex v({{k % 2 ? 1.6f : -1.6f, k / 2 ? 3.2f : 0, 0}}, {{0, 0, 1}},
                             {{255, 255, 255, 255}});
            v.texcoord = {
                {(variant * 32 + (k % 2 ? 31.5f : .5f)) / 256.f, k / 2 ? .015625f : .984375f}};
            m.vertices.push_back(v);
        }
        m.indices = {0, 1, 2, 1, 3, 2};
        meshes[variant] = rhythm_upload_mesh(m, "rhythm.character.mesh." + std::to_string(variant));
    }
    auto config = std::static_pointer_cast<gb::shape_3d_configuration>(
        fab->get_configuration_accessor()->get_shape_3d_configuration("rhythm_ship_character.xml"));
    for (int i = 0; i < 51; ++i)
    {
        auto e = gb::ces_entity::construct<gb::shape_3d>();
        e->tag = "rhythm.dancer." + std::to_string(i);
        auto g = e->get_component<gb::ces_geometry_3d_component>();
        g->set_mesh(meshes[i % 8]);
        g->bounding_mode = gb::ces_geometry_component::e_none;
        fab->add_materials(e, config->get_materials_configurations());
        auto c = e->add_component<ces_dance_character_component>();
        c->index = i;
        c->local_player = i == 0;
        c->dj = i == 50;
        c->base_position = i == 0 ? glm::vec3(0, 3.55f, 9)
                           : i == 50
                               ? glm::vec3(0, 6.4f, -29)
                               : glm::vec3(-12 + (i - 1) % 7 * 4.f, 3.55f, -12 + (i - 1) / 7 * 5.f);
        if (i > 0 && i < 50 &&
            glm::length(glm::vec2(c->base_position.x, c->base_position.z - 9)) < 3)
            c->base_position.x += 2.7f;
        c->phase = (i * 17 % 19) * .025f;
        c->amplitude = .12f + (i % 5) * .015f;
        c->speed = .96f + (i % 5) * .02f;
        e->position = c->base_position;
        root->add_child(e);
    }
}
} // namespace game
