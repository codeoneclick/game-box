//
//  ces_ai_car_replay_system.cpp
//  gbDemo
//
//  Created by serhii.s on 4/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_ai_car_replay_system.h"
#include "ces_car_replay_player_component.h"
#include "ces_car_simulator_component.h"
#include "ces_car_model_component.h"
#include "ces_car_descriptor_component.h"
#include "ces_box2d_body_component.h"
#include "ces_character_parts_component.h"
#include "ces_transformation_3d_component.h"
#include "ces_deferred_light_source_3d_component.h"
#include "ces_car_drift_state_component.h"
#include "ces_particle_emitter_component.h"
#include "ces_car_tire_trails_controller_component.h"
#include "ces_car_replay_record_component.h"
#include "ces_trail_component.h"
#include "game_object_3d.h"
#include "glm_extensions.h"
#include "std_extensions.h"

namespace game
{
    ces_ai_car_replay_system::ces_ai_car_replay_system()
    {
        ces_base_system::add_required_component_guid(m_ai_car_replay_components_mask, ces_car_replay_player_component::class_guid());
        ces_base_system::add_required_components_mask(m_ai_car_replay_components_mask);
    }
    
    ces_ai_car_replay_system::~ces_ai_car_replay_system()
    {
        
    }
    
    void ces_ai_car_replay_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_ai_car_replay_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_ai_car_replay_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            
            const auto box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
            const auto car_replay_player_component = entity->get_component<ces_car_replay_player_component>();
            car_replay_player_component->inc_tick_count();
            const auto record_data = car_replay_player_component->get_record_data();
            if (record_data)
            {
                glm::vec2 linear_velocity = record_data->get_linear_velocity();
                f32 angular_velocity = record_data->get_angular_velocity();
                box2d_body_component->linear_velocity = linear_velocity;
                box2d_body_component->angular_velocity = angular_velocity;
            }
            
            //const auto car_body = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_body));
            //car_body->rotation = glm::vec3(0.f, 0.f, -body_angle.get());
            
            //const auto car_tire_fl = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_fl_tire));
            //car_tire_fl->rotation = glm::vec3(0.f, glm::degrees(steer_angle), 0.f);
            
            //const auto car_tire_fr = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_fr_tire));
            //car_tire_fr->rotation = glm::vec3(0.f, glm::degrees(steer_angle), 0.f);
            
            //update_car_lights_direction(entity);
            //update_car_drift_state(entity);
            //update_car_tire_particles(entity);
            //update_car_tire_trails(entity);
        });
    }
    
    void ces_ai_car_replay_system::on_feed_end(f32 dt)
    {
        
    }
    
    void ces_ai_car_replay_system::update_car_lights_direction(const gb::ces_entity_shared_ptr& entity)
    {
        auto transformation_component = entity->get_component<gb::ces_transformation_3d_component>()->as_3d();
        auto car_rotation = glm::radians(transformation_component->get_rotation().y);
        
        glm::vec3 light_direction;
        const auto car_light_fl = entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_fl_light);
        auto deferred_light_source_component = car_light_fl->get_component<gb::ces_deferred_light_source_3d_component>();
        transformation_component = car_light_fl->get_component<gb::ces_transformation_3d_component>()->as_3d();
        auto light_rotation = glm::radians(transformation_component->get_rotation().y);
        light_direction.x = cosf(-glm::wrap_radians(car_rotation - M_PI_2 + light_rotation));
        light_direction.y = -.1f;
        light_direction.z = sinf(-glm::wrap_radians(car_rotation - M_PI_2 + light_rotation));
        deferred_light_source_component->set_direction(light_direction);
        
        const auto car_light_fr = entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_fr_light);
        deferred_light_source_component = car_light_fr->get_component<gb::ces_deferred_light_source_3d_component>();
        transformation_component = car_light_fr->get_component<gb::ces_transformation_3d_component>()->as_3d();
        light_rotation = glm::radians(transformation_component->get_rotation().y);
        light_direction.x = cosf(-glm::wrap_radians(car_rotation - M_PI_2 + light_rotation));
        light_direction.y = -.1f;
        light_direction.z = sinf(-glm::wrap_radians(car_rotation - M_PI_2 + light_rotation));
        deferred_light_source_component->set_direction(light_direction);
    }
    
    void ces_ai_car_replay_system::update_car_drift_state(const gb::ces_entity_shared_ptr& entity)
    {
        const auto car_drift_state_component = entity->get_component<ces_car_drift_state_component>();
        const auto car_simulator_component = entity->get_component<ces_car_simulator_component>();
        const auto car_descriptor_component = entity->get_component<ces_car_descriptor_component>();
        const auto car_model_component = entity->get_component<ces_car_model_component>();
        
        glm::vec2 lateral_force_front = car_simulator_component->lateral_force_front;
        glm::vec2 lateral_force_rear = car_simulator_component->lateral_force_rear;
        glm::vec2 velocity_wc = car_descriptor_component->velocity_wc;
        f32 lat_force_front_y = lateral_force_front.y;
        f32 lat_force_rear_y = lateral_force_rear.y;
        f32 velocity_length = glm::length(velocity_wc);
        f32 inv_max_grip = car_model_component->get_inv_max_grip();
        
        f32 last_front = car_drift_state_component->last_front;
        f32 last_rear = car_drift_state_component->last_rear;
        
        f32 lateral_forces_front = glm::mix(last_front, lat_force_front_y, .2f);
        last_front = lateral_forces_front;
        car_drift_state_component->last_front = last_front;
        lateral_forces_front = glm::clamp(fabsf(lateral_forces_front), 0.f, 1.f);
        
        f32 lateral_forces_rear = glm::mix(last_rear, lat_force_rear_y, .2f);
        last_rear = lateral_forces_rear;
        car_drift_state_component->last_rear = last_rear;
        lateral_forces_rear = glm::clamp(fabsf(lateral_forces_rear) * inv_max_grip, 0.f, 1.f);
        
        f32 drift_strength = glm::fixup((lateral_forces_front + lateral_forces_rear) * .5f);
        
        bool is_collided = car_drift_state_component->is_collided;
        bool is_drifting = car_drift_state_component->is_drifting;
        
        if (is_collided)
        {
            
        }
        else
        {
            
            if (!is_drifting)
            {
                if (drift_strength > .4f && velocity_length > 20.f)
                {
                    car_drift_state_component->is_drifting = true;
                    car_drift_state_component->is_collided = false;
                }
            }
            else
            {
                if (is_drifting && (drift_strength < .2f || velocity_length < 15.f))
                {
                    car_drift_state_component->is_drifting = false;
                    car_drift_state_component->is_collided = false;
                }
            }
        }
    }
    
    void ces_ai_car_replay_system::update_car_tire_particles(const gb::ces_entity_shared_ptr& entity)
    {
        const auto car_drift_state_component = entity->get_component<ces_car_drift_state_component>();
        
        const auto car_tire_rl_particles = entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_rl_tire_particles);
        auto particle_emitter_component = car_tire_rl_particles->get_component<gb::ces_particle_emitter_component>();
        particle_emitter_component->set_enabled(car_drift_state_component->is_drifting);
        
        const auto car_tire_rr_particles = entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_rr_tire_particles);
        particle_emitter_component = car_tire_rr_particles->get_component<gb::ces_particle_emitter_component>();
        particle_emitter_component->set_enabled(car_drift_state_component->is_drifting);
    }
    
    void ces_ai_car_replay_system::update_car_tire_trails(const gb::ces_entity_shared_ptr& entity)
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
            
            const auto car_tire_rl = entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_rl_tire);
            transformation_component = car_tire_rl->get_component<gb::ces_transformation_3d_component>()->as_3d();
            auto trail_component = tire_rl_trail->get_component<gb::ces_trail_component>();
            auto trail_position = transformation_component->get_absolute_position();
            trail_position.y += .4f;
            trail_component->set_new_segment_position(trail_position);
            trail_component->set_new_segment_direction(trail_direction);
            
            const auto car_tire_rr = entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_rr_tire);
            transformation_component = car_tire_rr->get_component<gb::ces_transformation_3d_component>()->as_3d();
            trail_component = tire_rr_trail->get_component<gb::ces_trail_component>();
            trail_position = transformation_component->get_absolute_position();
            trail_position.y += .4f;
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