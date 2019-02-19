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
#include "glm_extensions.h"

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
        ces_base_system::enumerate_entities_with_components(m_car_simulator_components_mask, [dt](const gb::ces_entity_shared_ptr& entity) {
            
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
            
            box2d_body_component->linear_velocity = velocity_wc;
            box2d_body_component->angular_velocity = angular_velocity;
            
            
            //std::cout<<"linear velocity: "<<velocity_wc.x<<", "<<velocity_wc.y<<std::endl;
            //std::cout<<"angular velocity: "<<angular_velocity<<std::endl;
            
            //std::cout<<"body angle: "<<box2d_body_angle<<std::endl;
        });
    }
    
    void ces_car_simulator_system::on_feed_end(f32 dt)
    {
        
    }
}
