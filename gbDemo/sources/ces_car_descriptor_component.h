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
    public:
        
        struct car_upgrade
        {
            i32 m_car_body_color_id = 1;
            i32 m_car_windshield_color_id = 1;
            f32 m_car_speed_upgrade_value = 0.f;
            f32 m_car_handling_upgrade_value = 0.f;
            f32 m_car_rigidity_upgrade_value = 0.f;
            
            bool is_equal(const std::shared_ptr<car_upgrade>& other);
            void apply(const std::shared_ptr<car_upgrade>& other);
        };
        
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
        
        f32 m_distance_to_activate_trigger = 15.f;
        i32 m_nearest_slow_motion_trigger_index = -1;
        i32 m_nearest_speed_up_trigger_index = -1;
        
        f32 m_max_damage = 3.f;
        f32 m_current_damage = 0.f;
        
        std::string m_racer_name = "";
        
        f32 m_motion_blur_effect_power = 0.f;
        
        std::shared_ptr<car_upgrade> m_car_upgrade = std::make_shared<car_upgrade>();
        std::shared_ptr<car_upgrade> m_car_upgrade_cache = std::make_shared<car_upgrade>();
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_car_descriptor_component, gb::ces_base_component::g_guids_container)
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
        
        std::property_ro<f32> distance_to_activate_trigger;
        std::property_rw<i32> nearest_slow_motion_trigger_index;
        std::property_rw<i32> nearest_speed_up_trigger_index;
        
        std::property_rw<f32> max_damage;
        std::property_rw<f32> current_damage;
        
        std::property_rw<f32> motion_blur_effect_power;
        
        std::shared_ptr<car_upgrade> get_car_upgrade() const;
        std::shared_ptr<car_upgrade> get_car_upgrade_cache() const;
    };
};

