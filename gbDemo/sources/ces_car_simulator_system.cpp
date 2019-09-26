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
#include "ces_car_parts_component.h"
#include "ces_transformation_3d_component.h"
#include "ces_deferred_light_source_3d_component.h"
#include "ces_car_drift_state_component.h"
#include "ces_particle_emitter_component.h"
#include "ces_car_tire_trails_controller_component.h"
#include "ces_car_replay_record_component.h"
#include "ces_car_ai_input_component.h"
#include "ces_render_technique_uniforms_component.h"
#include "ss_render_technique_custom_uniforms.h"
#include "ces_trail_component.h"
#include "game_object_3d.h"
#include "glm_extensions.h"
#include "std_extensions.h"
#include "ces_car_impact_component.h"

namespace game
{
    ces_car_simulator_system::ces_car_simulator_system()
    {
        ces_base_system::add_required_component_guid(m_main_car_simulator_components_mask, ces_car_simulator_component::class_guid());
        ces_base_system::add_required_component_guid(m_main_car_simulator_components_mask, ces_car_model_component::class_guid());
        ces_base_system::add_required_component_guid(m_main_car_simulator_components_mask, ces_car_input_component::class_guid());
        ces_base_system::add_required_component_guid(m_main_car_simulator_components_mask, ces_car_descriptor_component::class_guid());
        
        ces_base_system::add_required_components_mask(m_main_car_simulator_components_mask);
        
        ces_base_system::add_required_component_guid(m_ai_car_simulator_components_mask, ces_car_simulator_component::class_guid());
        ces_base_system::add_required_component_guid(m_ai_car_simulator_components_mask, ces_car_model_component::class_guid());
        ces_base_system::add_required_component_guid(m_ai_car_simulator_components_mask, ces_car_ai_input_component::class_guid());
        ces_base_system::add_required_component_guid(m_ai_car_simulator_components_mask, ces_car_descriptor_component::class_guid());
        
        ces_base_system::add_required_components_mask(m_ai_car_simulator_components_mask);
    }
    
    ces_car_simulator_system::~ces_car_simulator_system()
    {
        
    }
    
    void ces_car_simulator_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_car_simulator_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_main_car_simulator_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            
            update_main_car(entity, dt);
        });
        
        ces_base_system::enumerate_entities_with_components(m_ai_car_simulator_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            
            update_ai_car(entity, dt);
        });
    }
    
    void ces_car_simulator_system::on_feed_end(f32 dt)
    {
        
    }
    
    void ces_car_simulator_system::update_car(const gb::ces_entity_shared_ptr& entity, f32 dt, f32 throttle, f32 steer_angle, f32 brake)
    {
        const auto car_model_component = entity->get_component<ces_car_model_component>();
        const auto car_descriptor_component = entity->get_component<ces_car_descriptor_component>();
        const auto car_impact_component = entity->get_component<ces_car_impact_component>();
        const auto car_simulator_component = entity->get_component<ces_car_simulator_component>();
        const auto box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
        
        if (box2d_body_component->is_contacted)
        {
            f32 max_collision_interval = car_descriptor_component->max_colission_interval;
            f32 current_timestamp = std::get_tick_count();
            f32 last_collided_timestamp = car_descriptor_component->last_collided_timestamp;
            if (current_timestamp - last_collided_timestamp > max_collision_interval)
            {
                car_descriptor_component->last_collided_timestamp = current_timestamp;
            }
        }
        
        f32 max_force = car_model_component->get_max_force();
        if (car_impact_component && car_impact_component->is_speed_up_impact_exist())
        {
            max_force *= car_impact_component->get_speed_up_max_impact();
        }
        bool has_dir = false;
        bool has_steer = false;
        
        car_descriptor_component->brake = brake;
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
        
        throttle = car_descriptor_component->throttle;
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
        
        steer_angle = car_descriptor_component->steer_angle;
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
        
        glm::vec2 linear_impulse = glm::vec2(dt * acceleration_wc.x, dt * acceleration_wc.y);
        velocity_wc = car_descriptor_component->velocity_wc;
        velocity_wc.x += linear_impulse.x;
        velocity_wc.y += linear_impulse.y;
        
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
        
        /*const auto car_replay_record_component = entity->get_component<ces_car_replay_record_component>();
        car_replay_record_component->inc_tick_count();
        
        glm::vec2 previous_linear_velocity = box2d_body_component->linear_velocity;
        f32 previous_angular_velocity = box2d_body_component->angular_velocity;
        
        if (!std::is_f32_equal(previous_linear_velocity.x, velocity_wc.x) ||
            !std::is_f32_equal(previous_linear_velocity.y, velocity_wc.y) ||
            !std::is_f32_equal(previous_angular_velocity, angular_velocity))
        {
            car_replay_record_component->record(velocity_wc, angular_velocity);
        }*/
        
        box2d_body_component->angular_velocity = angular_velocity;
        box2d_body_component->linear_velocity = velocity_wc;
        
        
        f32 velocity_wc_length = glm::length(velocity_wc);
        f32 current_velocity_length_squared = velocity_wc_length * velocity_wc_length;
        f32 max_speed_squared = car_model_component->get_max_speed() * car_model_component->get_max_speed();
        f32 current_speed_factor = glm::clamp(current_velocity_length_squared / max_speed_squared, 0.f, 1.f);
        f32 front = -lateral_force_front.y * car_model_component->get_inv_max_grip();
        f32 rear = -lateral_force_rear.y * car_model_component->get_inv_max_grip();
        
        f32 amount = glm::clamp((front + rear) * .5f, -1.f, 1.f) / .75f;
        f32 sideangle_amount = (80.f * fabsf(amount)) * current_speed_factor * amount;
        
        f32 max = 15.f;
        sideangle_amount = glm::clamp(sideangle_amount, -max, max);
        
        f32 alpha = .05f;
        glm::interpolated_f32 side_angle = car_descriptor_component->side_angle;
        glm::interpolated_f32 body_angle = car_descriptor_component->body_angle;
        side_angle.set(sideangle_amount, alpha);
        body_angle.set(side_angle.get(), 1.f - current_speed_factor);
        car_descriptor_component->side_angle = side_angle;
        car_descriptor_component->body_angle = body_angle;
        
        const auto car_collision_container = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_collision_container));
        glm::vec2 box2d_position = box2d_body_component->position;
        f32 box2d_rotation = box2d_body_component->rotation;
        car_collision_container->position = glm::vec3(box2d_position.x, 0.f, box2d_position.y);
        car_collision_container->rotation = glm::vec3(0.f, glm::degrees(box2d_rotation), 0.f);
        
        const auto car_body = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_body));
        car_body->rotation = glm::vec3(0.f, 0.f, -body_angle.get());
        
        const auto car_fl_wheel_container = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_fl_wheel_container));
        car_fl_wheel_container->rotation = glm::vec3(0.f, glm::degrees(steer_angle), 0.f);
        
        const auto car_fr_wheel_container = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_fr_wheel_container));
        car_fr_wheel_container->rotation = glm::vec3(0.f, glm::degrees(steer_angle), 0.f);
        
        const auto car_fl_wheel = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_fl_tire));
        glm::vec3 wheel_rotation = car_fl_wheel->rotation;
        wheel_rotation.x += 7.f;
        car_fl_wheel->rotation = wheel_rotation;
        
        const auto car_fr_wheel = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_fr_tire));
        wheel_rotation = car_fr_wheel->rotation;
        wheel_rotation.x += 7.f;
        car_fr_wheel->rotation = wheel_rotation;
        
        const auto car_rl_wheel = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_rl_tire));
        wheel_rotation = car_rl_wheel->rotation;
        wheel_rotation.x += 7.f;
        car_rl_wheel->rotation = wheel_rotation;
        
        const auto car_rr_wheel = std::static_pointer_cast<gb::game_object_3d>(entity->get_component<ces_car_parts_component>()->get_part(ces_car_parts_component::parts::k_rr_tire));
        wheel_rotation = car_rr_wheel->rotation;
        wheel_rotation.x += 7.f;
        car_rr_wheel->rotation = wheel_rotation;
        
        update_car_drift_state(entity);
    }
    
    void ces_car_simulator_system::update_main_car(const gb::ces_entity_shared_ptr& entity, f32 dt)
    {
        const auto car_input_component = entity->get_component<ces_car_input_component>();
        update_car(entity, dt, car_input_component->throttle, car_input_component->steer_angle, car_input_component->brake);
    }
    
    void ces_car_simulator_system::update_ai_car(const gb::ces_entity_shared_ptr& entity, f32 dt)
    {
        const auto car_ai_input_component = entity->get_component<ces_car_ai_input_component>();
        update_car(entity, dt, car_ai_input_component->throttle, car_ai_input_component->steer_angle, car_ai_input_component->brake);
    }
    
    void ces_car_simulator_system::update_car_drift_state(const gb::ces_entity_shared_ptr& entity)
    {
        const auto car_drift_state_component = entity->get_component<ces_car_drift_state_component>();
        const auto car_simulator_component = entity->get_component<ces_car_simulator_component>();
        const auto car_descriptor_component = entity->get_component<ces_car_descriptor_component>();
        const auto car_model_component = entity->get_component<ces_car_model_component>();
        const auto box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
        
        bool is_contacted = box2d_body_component->is_contacted;
        car_drift_state_component->is_collided = is_contacted;
        if (is_contacted)
        {
            f32 max_collision_protection_time = car_drift_state_component->max_collision_protection_time;
            f32 last_collided_timestamp = car_drift_state_component->last_collided_timestamp;
            f32 current_timestamp = std::get_tick_count();
            if (current_timestamp - last_collided_timestamp > max_collision_protection_time)
            {
                car_drift_state_component->last_collided_timestamp = std::get_tick_count();
                f32 current_damage = car_descriptor_component->current_damage;
                current_damage += 1.f;
                car_descriptor_component->current_damage = current_damage;
            }
        }
        
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
            car_drift_state_component->is_drifting = false;
            car_drift_state_component->is_interrupted = true;
        }
        else
        {
            if (!is_drifting)
            {
                if (drift_strength > .4f && velocity_length > 20.f)
                {
                    car_drift_state_component->is_drifting = true;
                    car_drift_state_component->is_interrupted = false;
                    car_drift_state_component->last_drifting_time = std::get_tick_count();
                }
            }
            else
            {
                if (is_drifting && (drift_strength < .2f || velocity_length < 15.f))
                {
                    car_drift_state_component->is_drifting = false;
                }
            }
        }
    }
}
