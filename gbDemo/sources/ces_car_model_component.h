//
//  ces_car_model_component.hpp
//  gbDemo
//
//  Created by serhii.s on 2/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_car_model_component : public gb::ces_base_component
    {
    private:
        
        f32 m_wheelbase;
        f32 m_distance_to_front_axle;
        f32 m_distance_to_rear_axle;
        f32 m_mass;
        f32 m_inertia;
        f32 m_length;
        f32 m_width;
        f32 m_wheel_length;
        f32 m_wheel_width;
        
        f32 m_inv_mass;
        f32 m_inv_inertia;
        f32 m_density;
        f32 m_friction;
        f32 m_restitution;
        
        f32 m_drag;
        f32 m_resistance;
        f32 m_stiffness_front;
        f32 m_stiffness_rear;
        f32 m_max_grip;
        f32 m_inv_max_grip;
        f32 m_max_speed;
        f32 m_max_force;
        
        f32 m_weight;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_car_model_component, gb::ces_base_component::g_guids_container)
        ces_car_model_component();
        ~ces_car_model_component() = default;
        
        f32 get_wheelbase() const;
        f32 get_distance_to_front_axle() const;
        f32 get_distance_to_rear_axle() const;
        f32 get_mass() const;
        f32 get_inertia() const;
        f32 get_length() const;
        f32 get_width() const;
        f32 get_wheel_length() const;
        f32 get_wheel_width() const;
        
        f32 get_inv_mass() const;
        f32 get_inv_inertia() const;
        f32 get_density() const;
        f32 get_friction() const;
        f32 get_restitution() const;
        
        f32 get_drag() const;
        f32 get_resistance() const;
        f32 get_stiffness_front() const;
        f32 get_stiffness_rear() const;
        f32 get_max_grip() const;
        f32 get_inv_max_grip() const;
        f32 get_max_speed() const;
        f32 get_max_force() const;
        
        f32 get_weight() const;
        
        void set_max_force(f32 value);
    };
};
