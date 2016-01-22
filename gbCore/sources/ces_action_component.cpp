//
//  ces_action_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_action_component.h"

namespace gb
{
    ces_action_component::ces_action_component() :
    m_update_callback(nullptr)
    {
        m_type = e_ces_component_type_action;
    }
    
    ces_action_component::~ces_action_component()
    {
        
    }
    
    void ces_action_component::on_update(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        if(m_update_callback)
        {
            m_update_callback(entity, deltatime);
        }
    }
    
    void ces_action_component::set_update_callback(const t_update_callback& callback)
    {
        m_update_callback = callback;
    }
};