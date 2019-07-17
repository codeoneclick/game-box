//
//  ces_character_parts_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace game
{
    class ces_car_parts_component : public gb::ces_base_component
    {
    public:
        
        struct parts
        {
            static const std::string k_body;
            static const std::string k_bounds_part;
            static const std::string k_light_source_part;
            static const std::string k_fl_wheel_container;
            static const std::string k_fr_wheel_container;
            static const std::string k_fl_tire;
            static const std::string k_fr_tire;
            static const std::string k_rl_tire;
            static const std::string k_rr_tire;
            static const std::string k_fl_light;
            static const std::string k_fr_light;
            static const std::string k_bl_light;
            static const std::string k_br_light;
            static const std::string k_light_fl_direction;
            static const std::string k_light_fr_direction;
            static const std::string k_rl_tire_particles;
            static const std::string k_rr_tire_particles;
            static const std::string k_ui_name_label;
            static const std::string k_ui_speed_label;
            static const std::string k_ui_speed_value_label;
            static const std::string k_ui_drift_label;
            static const std::string k_ui_drift_value_label;
            static const std::string k_ui_rpm_label;
            static const std::string k_ui_rpm_value_label;
            static const std::string k_ui_direction_arrow;
            static const std::string k_ui_countdown_value_label;
            static const std::string k_collision_container;
        };
        
    private:
        
        gb::ces_entity_weak_ptr m_body_part;
        gb::ces_entity_weak_ptr m_bounds_part;
        gb::ces_entity_weak_ptr m_light_source_part;
        
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_parts;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_car_parts_component, gb::ces_base_component::g_guids_container)
        ces_car_parts_component();
        ~ces_car_parts_component();
        
        void setup(const gb::ces_entity_shared_ptr& body_part,
                   const gb::ces_entity_shared_ptr& bounds_part,
                   const gb::ces_entity_shared_ptr& ligth_source_part);
        
        void add_part(const gb::ces_entity_shared_ptr& part, const std::string& part_name);
        gb::ces_entity_shared_ptr get_part(const std::string& part_name) const;
        
        gb::ces_entity_shared_ptr get_body_part() const;
        gb::ces_entity_shared_ptr get_bounds_part() const;
        gb::ces_entity_shared_ptr get_light_source_part() const;
    };
};

