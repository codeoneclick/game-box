//
//  ces_character_controller_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
namespace ns
{
    class ces_character_controller_component : public gb::ces_base_component
    {
    public:
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&, f32)> t_update_callback;
        
    private:
        
    protected:
        
        t_update_callback m_update_callback;
        
    public:
        
        CTTI_CLASS_GUID(ces_character_controller_component, gb::ces_base_component::g_guids_container)
        ces_character_controller_component();
        ~ces_character_controller_component();
        
        void on_update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        void set_update_callback(const t_update_callback& callback);
    };
};
