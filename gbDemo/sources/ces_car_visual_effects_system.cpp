//
//  ces_character_visual_effects_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_car_visual_effects_system.h"
#include "ces_car_parts_component.h"
#include "ces_transformation_3d_component.h"
#include "ces_car_descriptor_component.h"
#include "ces_deferred_light_source_3d_component.h"
#include "ces_car_drift_state_component.h"
#include "ces_particle_emitter_component.h"
#include "ces_car_tire_trails_controller_component.h"
#include "ces_trail_component.h"
#include "ces_shader_uniforms_component.h"
#include "game_objects_custom_uniforms.h"
#include "camera_3d.h"

namespace game
{
    ces_car_visual_effects_system::ces_car_visual_effects_system()
    {
        ces_base_system::add_required_component_guid(m_car_visual_effects_components_mask, ces_car_descriptor_component::class_guid());
        ces_base_system::add_required_components_mask(m_car_visual_effects_components_mask);
    }
    
    ces_car_visual_effects_system::~ces_car_visual_effects_system()
    {
        
    }
    
    void ces_car_visual_effects_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_car_visual_effects_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_car_visual_effects_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            update_car_lights_direction(entity);
            update_car_tire_particles(entity);
            update_car_tire_trails(entity);
        });
    }
    
    void ces_car_visual_effects_system::on_feed_end(f32 dt)
    {
        
    }
    
    void ces_car_visual_effects_system::update_car_lights_direction(const gb::ces_entity_shared_ptr& entity)
    {
        auto transformation_component = entity->get_component<gb::ces_transformation_3d_component>()->as_3d();
        auto car_rotation = glm::radians(transformation_component->get_rotation().y);
        
        glm::vec3 light_direction;
        const auto car_light_fl = entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_light_fl_direction);
        auto deferred_light_source_component = car_light_fl->get_component<gb::ces_deferred_light_source_3d_component>();
        transformation_component = car_light_fl->get_component<gb::ces_transformation_3d_component>()->as_3d();
        auto light_rotation = glm::radians(transformation_component->get_rotation().y);
        light_direction.x = cosf(-glm::wrap_radians(car_rotation - M_PI_2 + light_rotation));
        light_direction.y = -.1f;
        light_direction.z = sinf(-glm::wrap_radians(car_rotation - M_PI_2 + light_rotation));
        deferred_light_source_component->set_direction(light_direction);
        
        const auto car_light_fr = entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_light_fr_direction);
        deferred_light_source_component = car_light_fr->get_component<gb::ces_deferred_light_source_3d_component>();
        transformation_component = car_light_fr->get_component<gb::ces_transformation_3d_component>()->as_3d();
        light_rotation = glm::radians(transformation_component->get_rotation().y);
        light_direction.x = cosf(-glm::wrap_radians(car_rotation - M_PI_2 + light_rotation));
        light_direction.y = -.1f;
        light_direction.z = sinf(-glm::wrap_radians(car_rotation - M_PI_2 + light_rotation));
        deferred_light_source_component->set_direction(light_direction);
    }
    
    void ces_car_visual_effects_system::update_car_tire_particles(const gb::ces_entity_shared_ptr& entity)
    {
        const auto car_drift_state_component = entity->get_component<ces_car_drift_state_component>();
        
        const auto car_tire_rl_particles = entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_rl_tire_particles);
        auto particle_emitter_component = car_tire_rl_particles->get_component<gb::ces_particle_emitter_component>();
        particle_emitter_component->set_enabled(car_drift_state_component->is_drifting);
        
        const auto car_tire_rr_particles = entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_rr_tire_particles);
        particle_emitter_component = car_tire_rr_particles->get_component<gb::ces_particle_emitter_component>();
        particle_emitter_component->set_enabled(car_drift_state_component->is_drifting);
    }
    
    void ces_car_visual_effects_system::update_car_tire_trails(const gb::ces_entity_shared_ptr& entity)
    {
        const auto car_drift_state_component = entity->get_component<ces_car_drift_state_component>();
        const auto car_tire_trails_controller_component = entity->get_component<ces_car_tire_trails_controller_component>();
        if (car_drift_state_component->is_drifting)
        {
            if (!car_tire_trails_controller_component->get_tire_rl_trail() && !car_tire_trails_controller_component->get_tire_rr_trail())
            {
                car_tire_trails_controller_component->enable_trails();
            }
            
            auto transformation_component = entity->get_component<gb::ces_transformation_3d_component>()->as_3d();
            auto car_rotation = glm::radians(transformation_component->get_rotation().y);
            glm::vec3 trail_direction;
            trail_direction.x = cosf(-glm::wrap_radians(car_rotation - M_PI_2));
            trail_direction.y = 0.f;
            trail_direction.z = sinf(-glm::wrap_radians(car_rotation - M_PI_2));
            
            const auto tire_rl_trail = car_tire_trails_controller_component->get_tire_rl_trail();
            const auto tire_rr_trail = car_tire_trails_controller_component->get_tire_rr_trail();
            
            const auto car_tire_rl = entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_rl_tire);
            transformation_component = car_tire_rl->get_component<gb::ces_transformation_3d_component>()->as_3d();
            auto trail_component = tire_rl_trail->get_component<gb::ces_trail_component>();
            auto trail_position = transformation_component->get_absolute_position();
            trail_position.y += .1f;
            trail_component->set_new_segment_position(trail_position);
            trail_component->set_new_segment_direction(trail_direction);
            
            const auto car_tire_rr = entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_rr_tire);
            transformation_component = car_tire_rr->get_component<gb::ces_transformation_3d_component>()->as_3d();
            trail_component = tire_rr_trail->get_component<gb::ces_trail_component>();
            trail_position = transformation_component->get_absolute_position();
            trail_position.y += .1f;
            trail_component->set_new_segment_position(trail_position);
            trail_component->set_new_segment_direction(trail_direction);
        }
        else
        {
            car_tire_trails_controller_component->disable_trails();
        }
        
        const auto disabled_trails = car_tire_trails_controller_component->get_disabled_trails();
        for (auto trail : disabled_trails)
        {
            if (!trail.expired())
            {
                const auto trail_component = trail.lock()->get_component<gb::ces_trail_component>();
                if (trail_component->is_expired())
                {
                    trail.lock()->remove_from_parent();
                }
            }
        }
    }
}
