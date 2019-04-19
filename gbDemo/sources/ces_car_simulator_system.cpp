//
//  ces_car_simulator_system.cpp
//  gbDemo
//
//  Created by serhii.s on 2/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_simulator_system.h"
#include "ces_car_simulator_component.h"
#include "ces_car_model_component.h"
#include "ces_car_input_component.h"
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
    ces_car_simulator_system::ces_car_simulator_system()
    {
        ces_base_system::add_required_component_guid(m_car_simulator_components_mask, ces_car_simulator_component::class_guid());
        ces_base_system::add_required_component_guid(m_car_simulator_components_mask, ces_car_model_component::class_guid());
        ces_base_system::add_required_component_guid(m_car_simulator_components_mask, ces_car_input_component::class_guid());
        ces_base_system::add_required_component_guid(m_car_simulator_components_mask, ces_car_descriptor_component::class_guid());
        
        ces_base_system::add_required_components_mask(m_car_simulator_components_mask);
    }
    
    ces_car_simulator_system::~ces_car_simulator_system()
    {
        
    }
    
    void ces_car_simulator_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_car_simulator_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_car_simulator_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            
            const auto car_model_component = entity->get_component<ces_car_model_component>();
            const auto car_descriptor_component = entity->get_component<ces_car_descriptor_component>();
            const auto car_input_component = entity->get_component<ces_car_input_component>();
            const auto car_simulator_component = entity->get_component<ces_car_simulator_component>();
            const auto box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
            
            f32 max_force = car_model_component->get_max_force();
            bool has_dir = false;
            bool has_steer = false;
            
            f32 brake = car_input_component->brake;
            bool input_updated = car_input_component->updated;
            car_descriptor_component->brake = brake;
            if (input_updated)
            {
                f32 throttle = car_input_component->throttle;
                if (glm::fixup(throttle) > 0.f)
                {
                    if (throttle < max_force)
                    {
                        car_descriptor_component->throttle = throttle;
                    }
                    else
                    {
                        car_descriptor_component->throttle = max_force;
                    }
                    has_dir = true;
                }
                else if (glm::fixup(throttle) < 0.f)
                {
                    if (throttle > -max_force)
                    {
                        car_descriptor_component->throttle = throttle;
                    }
                    else
                    {
                        car_descriptor_component->throttle = -max_force;
                    }
                    has_dir = true;
                }
                
                f32 steer_angle = car_input_component->steer_angle;
                if (glm::fixup(steer_angle) < 0.f)
                {
                    car_descriptor_component->steer_angle = steer_angle;
                    if (steer_angle < -M_PI_4)
                    {
                        car_descriptor_component->steer_angle = -M_PI_4;
                    }
                    
                    has_steer = true;
                }
                else if (glm::fixup(steer_angle) > 0.f)
                {
                    car_descriptor_component->steer_angle = steer_angle;
                    if (steer_angle > M_PI_4)
                    {
                        car_descriptor_component->steer_angle = M_PI_4;
                    }
                    
                    has_steer = true;
                }
            }
            
            if (!has_dir)
            {
                glm::vec2 velocity_wc = car_descriptor_component->velocity_wc;
                
                if (fabsf(velocity_wc.x) > .5f || fabsf(velocity_wc.y) > .5f)
                {
                    f32 throttle = car_descriptor_component->throttle;
                    if (!glm::is_zero(throttle))
                    {
                        f32 damping_throttle = car_simulator_component->damping_throttle;
                        throttle *= damping_throttle;
                        car_descriptor_component->throttle = throttle;
                    }
                    
                    f32 brake = car_descriptor_component->brake;
                    if (!glm::is_zero(brake))
                    {
                        f32 damping_throttle = car_simulator_component->damping_throttle;
                        brake *= damping_throttle;
                        car_descriptor_component->brake = brake;
                    }
                }
                else
                {
                    car_descriptor_component->velocity_wc = glm::vec2(0.f);
                    car_descriptor_component->angular_velocity = 0.f;
                    car_descriptor_component->brake = 0.f;
                    car_descriptor_component->throttle = 0.f;
                }
            }
            
            if (!has_steer)
            {
                car_descriptor_component->steer_angle = 0.f;
            }
            
            f32 throttle = car_descriptor_component->throttle;
            brake = car_descriptor_component->brake;
            car_descriptor_component->throttle = glm::clamp(throttle, -max_force, max_force);
            car_descriptor_component->brake = glm::clamp(brake, -max_force * 2.f, max_force * 2.f);
            
            b2Body* box2d_body = box2d_body_component->box2d_body;
            f32 box2d_body_angle = box2d_body->GetAngle();
            f32 sn = sinf(glm::wrap_radians(box2d_body_angle));
            f32 cs = cosf(glm::wrap_radians(box2d_body_angle));
            
            glm::vec2 velocity_wc = car_descriptor_component->velocity_wc;
            glm::vec2 velocity = car_simulator_component->velocity;
            velocity.x = glm::fixup(cs * velocity_wc.y + sn * velocity_wc.x);
            velocity.y = glm::fixup(-sn * velocity_wc.y + cs * velocity_wc.x);
            car_simulator_component->velocity = velocity;
            
            f32 wheelbase = car_model_component->get_wheelbase();
            f32 angular_velocity = car_descriptor_component->angular_velocity;
            f32 yaw_speed = wheelbase * .5f * angular_velocity;
            f32 side_slip = 0.f, rot_angle = 0.f;
            f32 slip_angle_front = 0.f, slip_angle_rear = 0.f;
            
            if (glm::is_zero(velocity.x))
            {
                rot_angle = 0.f;
                side_slip = 0.f;
                
                slip_angle_front = side_slip + rot_angle;
                slip_angle_rear = side_slip - rot_angle;
            }
            else
            {
                rot_angle = atan2(yaw_speed, velocity.x);
                side_slip = atan2(velocity.y, velocity.x);
                
                f32 steer_angle = car_descriptor_component->steer_angle;
                slip_angle_front = side_slip + rot_angle - steer_angle;
                slip_angle_rear = side_slip - rot_angle;
            }
            
            glm::vec2 flatf = car_simulator_component->flatf;
            glm::vec2 flatr = car_simulator_component->flatr;
            glm::vec2 lateral_force_front = car_simulator_component->lateral_force_front;
            glm::vec2 lateral_force_rear = car_simulator_component->lateral_force_rear;
            f32 stiffness_front = car_model_component->get_stiffness_front();
            f32 stiffness_rear = car_model_component->get_stiffness_rear();
            f32 max_grip = car_model_component->get_max_grip();
            
            flatf.x = 0.f;
            flatf.y = stiffness_front * slip_angle_front;
            flatf.y = std::min(max_grip, flatf.y);
            flatf.y = std::max(-max_grip, flatf.y);
            lateral_force_front = flatf;
            flatf.y *= car_model_component->get_weight();
            
            car_simulator_component->flatf = flatf;
            car_simulator_component->lateral_force_front = lateral_force_front;
            
            flatr.x = 0.f;
            flatr.y = stiffness_rear * slip_angle_rear;
            flatr.y = std::min(max_grip, flatr.y);
            flatr.y = std::max(-max_grip, flatr.y);
            lateral_force_rear = flatr;
            flatr.y *= car_model_component->get_weight();
            
            car_simulator_component->flatr = flatr;
            car_simulator_component->lateral_force_rear = lateral_force_rear;
            
            f32 current_sign = car_simulator_component->current_sign;
            current_sign = glm::sign(velocity.x);
            car_simulator_component->current_sign = current_sign;
            
            glm::vec2 ftraction = car_simulator_component->ftraction;
            ftraction = glm::vec2(100.f * (throttle - brake * current_sign), 0.f);
            
            f32 torque = car_model_component->get_distance_to_front_axle() * flatf.y - car_model_component->get_distance_to_rear_axle() * flatr.y;
            torque = glm::fixup(torque);
            
            glm::vec2 resistance = car_simulator_component->resistance;
            resistance.x = -(car_model_component->get_resistance() * velocity.x + car_model_component->get_drag() * velocity.x * fabsf(velocity.x));
            resistance.y = -(car_model_component->get_resistance() * velocity.y + car_model_component->get_drag() * velocity.y * fabsf(velocity.y));
            car_simulator_component->resistance = resistance;
            
            f32 steer_angle = car_descriptor_component->steer_angle;
            glm::vec2 force = car_simulator_component->force;
            force.x = ftraction.x + sinf(steer_angle) * flatf.x + flatr.x + resistance.x;
            force.y = ftraction.y + cosf(steer_angle) * flatf.y + flatr.y + resistance.y;
            car_simulator_component->force = force;
            
            glm::vec2 acceleration = car_simulator_component->acceleration;
            acceleration = glm::vec2(glm::fixup(force.x * car_model_component->get_inv_mass()),
                                     glm::fixup(force.y * car_model_component->get_inv_mass()));
            car_simulator_component->acceleration = acceleration;
            
            glm::vec2 acceleration_wc = car_simulator_component->acceleration_wc;
            acceleration_wc.x = glm::fixup(cs * acceleration.y + sn * acceleration.x);
            acceleration_wc.y = glm::fixup(-sn * acceleration.y + cs * acceleration.x);
            car_simulator_component->acceleration_wc = acceleration_wc;
            
            velocity_wc = car_descriptor_component->velocity_wc;
            velocity_wc.x += dt * acceleration_wc.x;
            velocity_wc.y += dt * acceleration_wc.y;
            velocity_wc.x = glm::fixup(velocity_wc.x);
            velocity_wc.y = glm::fixup(velocity_wc.y);
            velocity_wc = glm::truncate(velocity_wc, car_model_component->get_max_speed());
            car_descriptor_component->velocity_wc = velocity_wc;
            
            f32 angular_acceleration = torque * car_model_component->get_inv_inertia();
            angular_velocity = car_descriptor_component->angular_velocity;
            angular_velocity += dt * angular_acceleration;
            angular_velocity = glm::fixup(angular_velocity);
            car_descriptor_component->angular_velocity = angular_velocity;
            
            f32 degree_of_rotation_per_frame = ((velocity.length() * dt) / car_model_component->get_wheel_length()) * 360.f;
            f32 degree_of_rotation_per_second = degree_of_rotation_per_frame * 30.f;
            f32 rps_wheel = degree_of_rotation_per_second / 360.f;
            car_simulator_component->rpm_wheel = rps_wheel * 60.f;
            
            const auto car_replay_record_component = entity->get_component<ces_car_replay_record_component>();
            car_replay_record_component->inc_tick_count();
            
            glm::vec2 previous_linear_velocity = box2d_body_component->linear_velocity;
            f32 previous_angular_velocity = box2d_body_component->angular_velocity;
            
            if (!std::is_f32_equal(previous_linear_velocity.x, velocity_wc.x) ||
                !std::is_f32_equal(previous_linear_velocity.y, velocity_wc.y) ||
                !std::is_f32_equal(previous_angular_velocity, angular_velocity))
            {
                car_replay_record_component->record(velocity_wc, angular_velocity);
            }
            
            box2d_body_component->linear_velocity = velocity_wc;
            box2d_body_component->angular_velocity = angular_velocity;
            
            f32 velocity_wc_length = glm::length(velocity_wc);
            f32 current_velocity_length_squared = velocity_wc_length * velocity_wc_length;
            f32 max_speed_squared = car_model_component->get_max_speed() * car_model_component->get_max_speed();
            f32 current_speed_factor = glm::clamp(current_velocity_length_squared / max_speed_squared, 0.f, 1.f);
            f32 front = -lateral_force_front.y * car_model_component->get_inv_max_grip();
            f32 rear = -lateral_force_rear.y * car_model_component->get_inv_max_grip();
            
            f32 amount = glm::clamp((front + rear) * .5f, -1.f, 1.f) / .75f;
            f32 sideangle_amount = (80.f * fabsf(amount)) * current_speed_factor * amount;
            
            f32 max = 30.f;
            sideangle_amount = glm::clamp(sideangle_amount, -max, max);
            
            f32 alpha = .05f;
            glm::interpolated_f32 side_angle = car_descriptor_component->side_angle;
            glm::interpolated_f32 body_angle = car_descriptor_component->body_angle;
            side_angle.set(sideangle_amount, alpha);
            body_angle.set(side_angle.get(), 1.f - current_speed_factor);
            car_descriptor_component->side_angle = side_angle;
            car_descriptor_component->body_angle = body_angle;
            
            const auto car_body = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_body));
            car_body->rotation = glm::vec3(0.f, 0.f, -body_angle.get());
            
            const auto car_tire_fl = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_fl_tire));
            car_tire_fl->rotation = glm::vec3(0.f, glm::degrees(steer_angle), 0.f);
            
            const auto car_tire_fr = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_fr_tire));
            car_tire_fr->rotation = glm::vec3(0.f, glm::degrees(steer_angle), 0.f);
            
            update_car_lights_direction(entity);
            update_car_drift_state(entity);
            update_car_tire_particles(entity);
            update_car_tire_trails(entity);
        });
    }
    
    void ces_car_simulator_system::on_feed_end(f32 dt)
    {
        
    }
    
    void ces_car_simulator_system::update_car_lights_direction(const gb::ces_entity_shared_ptr& entity)
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
    
    void ces_car_simulator_system::update_car_drift_state(const gb::ces_entity_shared_ptr& entity)
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
    
    void ces_car_simulator_system::update_car_tire_particles(const gb::ces_entity_shared_ptr& entity)
    {
        const auto car_drift_state_component = entity->get_component<ces_car_drift_state_component>();
        
        const auto car_tire_rl_particles = entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_rl_tire_particles);
        auto particle_emitter_component = car_tire_rl_particles->get_component<gb::ces_particle_emitter_component>();
        particle_emitter_component->set_enabled(car_drift_state_component->is_drifting);
        
        const auto car_tire_rr_particles = entity->get_component<ces_character_parts_component>()->get_part(ces_character_parts_component::parts::k_rr_tire_particles);
        particle_emitter_component = car_tire_rr_particles->get_component<gb::ces_particle_emitter_component>();
        particle_emitter_component->set_enabled(car_drift_state_component->is_drifting);
    }
    
    void ces_car_simulator_system::update_car_tire_trails(const gb::ces_entity_shared_ptr& entity)
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
