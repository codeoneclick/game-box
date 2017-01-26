//
//  command_processor.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "net_declarations.h"

namespace gb
{
    namespace net
    {
        class command_processor
        {
        public:
            
            typedef std::function<command_shared_ptr(const std::shared_ptr<std::streambuf>& buffer)> command_creator_t;
            typedef std::function<void(const command_shared_ptr&)> command_callback_t;
            
        private:
            
        protected:
            
            static std::map<i32, command_creator_t> m_command_creators;
            std::map<i32, std::list<command_callback_t>> m_command_callbacks;
            
        public:
            
            command_processor();
            ~command_processor();
            
            static command_shared_ptr deserialize(i32 command_id, const std::shared_ptr<std::streambuf>& buffer);
            
            void register_command_creator(i32 command_id, const command_creator_t& creator);
            
            std::list<command_callback_t>::iterator register_command_callback(i32 command_id,
                                                                              const command_callback_t& callback);
            void unregister_command_callback(i32 command_id, const std::list<command_callback_t>::iterator& iterator);
            
            void execute_callback_for_command(const command_shared_ptr& command);
        };
    };
};

