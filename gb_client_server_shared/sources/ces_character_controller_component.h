//
//  ces_character_controller_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_character_controller_component : public gb::ces_base_component
    {
    public:
        
        enum e_mode
        {
            unknown = -1,
            main = 1,
            ai
        };
        
    public:
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&)> dead_callback_t;
        typedef std::function<void(const gb::ces_entity_shared_ptr&, f32)> health_changed_callback_t;
        typedef std::function<void(const gb::ces_entity_shared_ptr&, const gb::ces_entity_shared_ptr&)> kill_callback_t;
        
    private:
        
    protected:
        
        e_mode m_mode;
        f32 m_health;
        
        dead_callback_t m_dead_callback;
        health_changed_callback_t m_health_changed_callback;
        kill_callback_t m_kill_callback;
        
        information_bubble_controller_weak_ptr m_information_bubble_controller;
        bloodprint_controller_weak_ptr m_bloodprint_controller;
        footprint_controller_weak_ptr m_footprint_controller;
        
        gb::ces_entity_weak_ptr m_auto_aim_target;
  
    public:
        
        CTTI_CLASS_GUID(ces_character_controller_component, gb::ces_base_component::g_guids_container)
        ces_character_controller_component();
        ~ces_character_controller_component();
        
        std::property_rw<information_bubble_controller_shared_ptr> information_bubble_controller;
        std::property_rw<bloodprint_controller_shared_ptr> bloodprint_controller;
        std::property_rw<footprint_controller_shared_ptr> footprint_controller;
        
        std::property_rw<e_mode> mode;
        
        std::property_ro<f32> health;
        void add_health(const gb::ces_entity_shared_ptr& owner, f32 health);
        void set_health_changed_callback(const health_changed_callback_t& callback);
        void reset_health();
        std::property_ro<bool> is_dead;
        
        void on_dead(const gb::ces_entity_shared_ptr& owner);
        void set_dead_callback(const dead_callback_t& callback);
        
        void on_kill(const gb::ces_entity_shared_ptr& owner, const gb::ces_entity_shared_ptr& target);
        void set_kill_callback(const kill_callback_t& callback);
        
        void set_auto_aim_target(const gb::ces_entity_shared_ptr& target);
        gb::ces_entity_shared_ptr get_auto_aim_target() const;
    };
};
