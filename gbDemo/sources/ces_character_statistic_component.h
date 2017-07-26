//
//  ces_character_statistic_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 6/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_character_statistic_component : public gb::ces_base_component
    {
    public:
        
        enum e_parameter
        {
            e_parameter_hp = 0,
            e_parameter_damage,
            e_parameter_move_speed,
            e_parameter_attack_speed,
            e_parameter_attack_distance,
            e_parameter_chase_start_distance,
            e_parameter_chase_end_distance,
            e_parameters_max
        };
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&, e_parameter, f32)> on_parameter_changed_callback_t;
        
    private:
        
        std::vector<std::tuple<gb::ces_entity_weak_ptr, on_parameter_changed_callback_t>> m_on_parameter_changed_callbacks;
        void on_parameter_changed(e_parameter parameter, f32 delta);
        
    protected:
        
        std::array<f32, e_parameters_max> m_initial_parameters;
        std::array<f32, e_parameters_max> m_current_parameters;
        
    public:
        
        CTTI_CLASS_GUID(ces_character_statistic_component, gb::ces_base_component::g_guids_container)
        ces_character_statistic_component();
        ~ces_character_statistic_component();
        
        void setup(f32 max_health,
                   f32 max_move_speed,
                   f32 max_attack_speed,
                   f32 max_damage,
                   f32 attack_distance);
        
        void setup(f32 chase_start_distance,
                   f32 chase_end_distance);
        
        std::property_rw<f32> current_health;
        std::property_rw<f32> current_move_speed;
        std::property_rw<f32> current_attack_speed;
        std::property_rw<f32> current_damage;
        std::property_rw<f32> current_attack_distance;
        std::property_rw<f32> current_chase_start_distance;
        std::property_rw<f32> current_chase_end_distance;
        
        std::property_ro<f32> max_health;
        std::property_ro<f32> max_move_speed;
        std::property_ro<f32> max_attack_speed;
        std::property_ro<f32> max_damage;
        std::property_ro<f32> max_attack_distance;
        std::property_rw<f32> max_chase_start_distance;
        std::property_rw<f32> max_chase_end_distance;
        
        std::property_ro<bool> is_dead;
        std::property_ro<f32> current_health_percents;
        
        void add_on_parameter_changed_callback(const gb::ces_entity_shared_ptr& owner, const on_parameter_changed_callback_t& callback);
        void remove_on_parameter_changed_callback(const gb::ces_entity_shared_ptr& owner);
    };
};
