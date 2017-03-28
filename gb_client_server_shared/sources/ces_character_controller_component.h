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
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&)> spawn_callback_t;
        typedef std::function<void(const gb::ces_entity_shared_ptr&, f32)> health_changed_callback_t;
        
    private:
        
    protected:
        
        e_mode m_mode;
        f32 m_health;
        spawn_callback_t m_spawn_callback;
        health_changed_callback_t m_health_changed_callback;
        
        information_bubble_controller_weak_ptr m_information_bubble_controller;
        bloodprint_controller_weak_ptr m_bloodprint_controller;
        footprint_controller_weak_ptr m_footprint_controller;
  
    public:
        
        CTTI_CLASS_GUID(ces_character_controller_component, gb::ces_base_component::g_guids_container)
        ces_character_controller_component();
        ~ces_character_controller_component();
        
        std::property_rw<information_bubble_controller_shared_ptr> information_bubble_controller;
        std::property_rw<bloodprint_controller_shared_ptr> bloodprint_controller;
        std::property_rw<footprint_controller_shared_ptr> footprint_controller;
        
        std::property_rw<e_mode> mode;
        
        std::property_ro<f32> health;
        void add_health(const gb::ces_entity_shared_ptr& entity, f32 health);
        
        void on_spawn(const gb::ces_entity_shared_ptr& entity);
        void set_spawn_callback(const spawn_callback_t& callback);
        
        void set_health_changed_callback(const health_changed_callback_t& callback);
    };
};
