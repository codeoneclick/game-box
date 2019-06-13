//
//  ces_car_descriptor_component.hpp
//  gbDemo
//
//  Created by serhii.s on 2/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"
#include "glm_extensions.h"

namespace game
{
    class ces_car_descriptor_component : public gb::ces_base_component
    {
    private:
    
        glm::vec2 m_position_wc = glm::vec2(0.f);
        glm::vec2 m_velocity_wc = glm::vec2(0.f);
        
        f32 m_angular_velocity = 0.f;
        f32 m_steer_angle = 0.f;
        
        f32 m_throttle = 0.f;
        f32 m_brake = 0.f;
        
        glm::interpolated_f32 m_side_angle;
        glm::interpolated_f32 m_body_angle;
        
        f32 m_last_collided_timestamp = 0.f;
        f32 m_max_colission_interval = 5000.f;
        
        i32 m_lap_passed_index = 0;
        i32 m_checkpoint_passed_index = 0;
        f32 m_distance_to_next_checkpoint = 0.f;
        
        i32 m_place = 0;
        
        f32 m_distance_to_activate_motion_trigger = 15.f;
        i32 m_nearest_slow_motion_trigger_index = -1;
        
        f32 m_max_damage = 3.f;
        f32 m_current_damage = 0.f;
        
        std::string m_racer_name = "";
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_car_descriptor_component, gb::ces_base_component::g_guids_container)
        ces_car_descriptor_component();
        ~ces_car_descriptor_component() = default;
        
        std::property_rw<glm::vec2> position_wc;
        std::property_rw<glm::vec2> velocity_wc;
        
        std::property_rw<f32> angular_velocity;
        std::property_rw<f32> steer_angle;
        
        std::property_rw<f32> throttle;
        std::property_rw<f32> brake;
        
        std::property_rw<glm::interpolated_f32> side_angle;
        std::property_rw<glm::interpolated_f32> body_angle;
        
        std::property_rw<f32> last_collided_timestamp;
        std::property_ro<f32> max_colission_interval;
        
        std::property_rw<i32> lap_passed_index;
        std::property_rw<i32> checkpoint_passed_index;
        std::property_rw<f32> distance_to_next_checkpoint;
        
        std::property_rw<std::string> racer_name;
        
        std::property_rw<i32> place;
        
        std::property_ro<f32> distance_to_activate_motion_trigger;
        std::property_rw<i32> nearest_slow_motion_trigger_index;
        
        std::property_ro<f32> max_damage;
        std::property_rw<f32> current_damage;
    };
};

