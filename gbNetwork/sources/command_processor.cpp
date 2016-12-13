//
//  command_processor.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "command_processor.h"
#include "command.h"

namespace gb
{
    namespace net
    {
        std::map<i32, command_processor::command_creator_t> command_processor::m_command_creators;
        
        command_processor::command_processor()
        {
            
        }
        
        command_processor::~command_processor()
        {
            
        }
        
        command_shared_ptr command_processor::deserialize(i32 command_id, asio::streambuf&& buffer)
        {
            command_shared_ptr command = nullptr;
            const auto& iterator = m_command_creators.find(command_id);
            assert(iterator != m_command_creators.end());
            if(iterator != m_command_creators.end())
            {
                command = iterator->second(std::move(buffer));
            }
            return command;
        }
        
        void command_processor::register_command_creator(i32 command_id, const command_creator_t& creator)
        {
            const auto& iterator = m_command_creators.find(command_id);
            assert(iterator == m_command_creators.end());
            if(iterator == m_command_creators.end())
            {
                m_command_creators.insert(std::make_pair(command_id, creator));
            }
        }
    }
}
