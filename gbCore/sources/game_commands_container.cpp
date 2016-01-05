//
//  game_commands_container.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "game_commands_container.h"

namespace gb
{
    game_commands_container::~game_commands_container(void)
    {
        m_commands.clear();
    }
    
    void game_commands_container::add_command( const std::string& guid, const game_command_i_shared_ptr& command)
    {
        m_commands.insert(std::make_pair(guid, command));
    };
    
    void game_commands_container::remove_command(const std::string& guid)
    {
        auto command = m_commands.find(guid);
        if(command != m_commands.end())
        {
            m_commands.erase(command);
        }
    }
}