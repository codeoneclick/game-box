//
//  ces_character_controller_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

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
        
    private:
        
    protected:
        
        e_mode m_mode;
        f32 m_health;
        spawn_callback_t m_spawn_callback;
  
    public:
        
        CTTI_CLASS_GUID(ces_character_controller_component, gb::ces_base_component::g_guids_container)
        ces_character_controller_component();
        ~ces_character_controller_component();
        
        std::property_rw<e_mode> mode;
        
        std::property_ro<f32> health;
        void add_health(f32 health);
        
        void on_spawn(const gb::ces_entity_shared_ptr& entity);
        void set_spawn_callback(const spawn_callback_t& callback);
    };
};
