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
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&, f32)> on_health_changed_callback_t;
        
    private:
        
        on_health_changed_callback_t m_on_health_changed_callback;
        
    protected:
        
        f32 m_max_health;
        f32 m_current_health;
        f32 m_max_move_speed;
        f32 m_current_move_speed;
        f32 m_max_attack_speed;
        f32 m_current_attack_speed;
        f32 m_max_damage;
        f32 m_current_damage;
        
    public:
        
        CTTI_CLASS_GUID(ces_character_statistic_component, gb::ces_base_component::g_guids_container)
        ces_character_statistic_component();
        ~ces_character_statistic_component();
        
        void setup(f32 max_health,
                   f32 max_move_speed,
                   f32 max_attack_speed,
                   f32 max_damage);
        
        std::property_rw<f32> current_health;
        std::property_rw<f32> current_move_speed;
        std::property_rw<f32> current_attack_speed;
        std::property_rw<f32> current_damage;
        
        std::property_ro<f32> max_health;
        std::property_ro<f32> max_move_speed;
        std::property_ro<f32> max_attack_speed;
        std::property_ro<f32> max_damage;
        
        std::property_ro<bool> is_dead;
        std::property_ro<f32> current_health_percents;
        
        void on_health_changed(const gb::ces_entity_shared_ptr& owner, f32 delta);
        void set_on_health_changed_callback(const on_health_changed_callback_t& callback);
    };
};
