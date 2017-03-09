//
//  ces_character_controller_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_character_controller_component.h"

namespace game
{
    ces_character_controller_component::ces_character_controller_component() :
    m_update_callback(nullptr)
    {
        
    }
    
    ces_character_controller_component::~ces_character_controller_component()
    {
        
    }
    
    void ces_character_controller_component::on_update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        if(m_update_callback)
        {
            m_update_callback(entity, deltatime);
        }
    }
    
    void ces_character_controller_component::set_update_callback(const t_update_callback& callback)
    {
        m_update_callback = callback;
    }
}
