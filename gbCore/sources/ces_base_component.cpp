//
//  ces_base_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_base_component.h"
#include "game_commands_container.h"

namespace gb
{
    ces_base_component::ces_base_component() :
    m_type(e_ces_component_type_undefined),
    m_event_listeners(std::make_shared<game_commands_container>())
    {
        
    }
    
    i32 ces_base_component::get_type() const
    {
        return m_type;
    }
    
    void ces_base_component::add_event_listener(const std::string& guid, const game_command_i_shared_ptr& command)
    {
        m_event_listeners->add_command(guid, command);
    }
    
    void ces_base_component::remove_event_listener(const std::string& guid)
    {
        m_event_listeners->remove_command(guid);
    }
};