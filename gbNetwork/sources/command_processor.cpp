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
        
        command_shared_ptr command_processor::deserialize(i32 command_id, const std::shared_ptr<std::streambuf>& buffer)
        {
            command_shared_ptr command = nullptr;
            const auto& iterator = m_command_creators.find(command_id);
            if(iterator != m_command_creators.end())
            {
                command = iterator->second(buffer);
            }
            else
            {
                std::cerr<<"command "<<command_id<<" does not exist"<<std::endl;
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
        
        std::list<command_processor::command_callback_t>::iterator command_processor::register_command_callback(i32 command_id,
                                                                          const command_callback_t& callback)
        {
            std::list<command_callback_t>::iterator iterator;
            const auto& callbacks_container = m_command_callbacks.find(command_id);
            if(callbacks_container == m_command_callbacks.end())
            {
                std::list<command_callback_t> callbacks;
                iterator = callbacks.emplace(callbacks.begin(), callback);
                m_command_callbacks.insert(std::make_pair(command_id, callbacks));
            }
            else
            {
                iterator = callbacks_container->second.emplace(callbacks_container->second.begin(), callback);
            }
            return iterator;
        }
        
        void command_processor::unregister_command_callback(i32 command_id,
                                                            const std::list<command_callback_t>::iterator& iterator)
        {
            const auto& callbacks_container = m_command_callbacks.find(command_id);
            if(callbacks_container != m_command_callbacks.end())
            {
                callbacks_container->second.erase(iterator);
            }
        }
        
        void command_processor::execute_callback_for_command(const command_shared_ptr& command)
        {
            ui32 command_id = command->command_id;
            const auto& callbacks_container = m_command_callbacks.find(command_id);
            if(callbacks_container != m_command_callbacks.end())
            {
                for(const auto& callback :callbacks_container->second)
                {
                    callback(command);
                }
            }
        }
        
        void command_processor::unregister_all_cammand_creators()
        {
            m_command_creators.clear();
        }
    }
}
