//
//  ces_car_model_component.cpp
//  gbDemo
//
//  Created by serhii.s on 2/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_model_component.h"

namespace game
{
    glm::u8vec4 ces_car_model_component::k_car_color_1 = glm::u8vec4(56, 120, 103, 255);
    glm::u8vec4 ces_car_model_component::k_car_color_2 = glm::u8vec4(220, 218, 177, 255);
    glm::u8vec4 ces_car_model_component::k_car_color_3 = glm::u8vec4(135, 38, 21, 255);
    glm::u8vec4 ces_car_model_component::k_car_color_4 = glm::u8vec4(207, 159, 1, 255);
    
    ces_car_model_component::ces_car_model_component()
    {
        m_mass = 1500.f;
        m_inv_mass = 1.f / m_mass;
        m_inertia = 1500.f;
        m_inv_inertia = 1.f / m_inertia;
        
        m_distance_to_front_axle = 1.f;
        m_distance_to_rear_axle = 1.f;
        m_width = 2.f;
        m_length = 3.8f;
        
        m_wheel_length = .7f;
        m_wheel_width = .3f;
        
        m_weight = m_mass * 9.8f * .5f;
        m_wheelbase = m_distance_to_front_axle + m_distance_to_rear_axle;
        
        m_drag = 20.f;
        m_resistance = 30.f;
        m_stiffness_rear = -3.8f;
        m_stiffness_front = -3.5f;
        m_max_grip = 4.5f;
        m_inv_max_grip = 1.f / m_max_grip;
        
        m_max_speed = 45.f;
        m_max_force = 270.f;
        
        m_density = 0.f;
        m_friction = 0.02f;
        m_restitution = .0f;
    }
    
    f32 ces_car_model_component::get_wheelbase() const
    {
        return m_wheelbase;
    }
    
    f32 ces_car_model_component::get_distance_to_front_axle() const
    {
        return m_distance_to_front_axle;
    }
    
    f32 ces_car_model_component::get_distance_to_rear_axle() const
    {
        return m_distance_to_rear_axle;
    }
    
    f32 ces_car_model_component::get_mass() const
    {
        return m_mass;
    }
    
    f32 ces_car_model_component::get_inertia() const
    {
        return m_inertia;
    }
    
    f32 ces_car_model_component::get_length() const
    {
        return m_length;
    }
    
    f32 ces_car_model_component::get_width() const
    {
        return m_width;
    }
    
    f32 ces_car_model_component::get_wheel_length() const
    {
        return m_wheel_length;
    }
    
    f32 ces_car_model_component::get_wheel_width() const
    {
        return m_wheel_width;
    }
    
    f32 ces_car_model_component::get_inv_mass() const
    {
        return m_inv_mass;
    }
    
    f32 ces_car_model_component::get_inv_inertia() const
    {
        return m_inv_inertia;
    }
    
    f32 ces_car_model_component::get_density() const
    {
        return m_density;
    }
    
    f32 ces_car_model_component::get_friction() const
    {
        return m_friction;
    }
    
    f32 ces_car_model_component::get_restitution() const
    {
        return m_restitution;
    }
    
    f32 ces_car_model_component::get_drag() const
    {
        return m_drag;
    }
    
    f32 ces_car_model_component::get_resistance() const
    {
        return m_resistance;
    }
    
    f32 ces_car_model_component::get_stiffness_front() const
    {
        return m_stiffness_front;
    }
    
    f32 ces_car_model_component::get_stiffness_rear() const
    {
        return m_stiffness_rear;
    }
    
    f32 ces_car_model_component::get_max_grip() const
    {
        return m_max_grip;
    }
    
    f32 ces_car_model_component::get_inv_max_grip() const
    {
        return m_inv_max_grip;
    }
    
    f32 ces_car_model_component::get_max_speed() const
    {
        return m_max_speed;
    }
    
    f32 ces_car_model_component::get_max_force() const
    {
        return m_max_force;
    }
    
    f32 ces_car_model_component::get_weight() const
    {
        return m_weight;
    }
    
    void ces_car_model_component::set_max_force(f32 value)
    {
        m_max_speed = value;
    }
}
