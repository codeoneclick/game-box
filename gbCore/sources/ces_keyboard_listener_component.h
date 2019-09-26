//
//  ces_keyboard_listener_component.h
//  gbCore
//
//  Created by serhii.s on 9/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//
#pragma once

#include "ces_base_component.h"
#include "input_context.h"

namespace gb
{
    class ces_keyboard_listener_component : public ces_base_component
    {
    
    public:
        
        typedef std::function<void(const ces_entity_shared_ptr&,
        e_input_state,
        i32 key)> t_callback;
        
    private:
        
         std::map<std::string, t_callback> m_callbacks;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_keyboard_listener_component, ces_base_component::g_guids_container)
        ces_keyboard_listener_component() = default;
        ~ces_keyboard_listener_component() = default;
        
        std::string add_callback(const t_callback& callback);
        void remove_callback(const std::string& guid);
        
        std::list<t_callback> get_callbacks() const;
    };
};
