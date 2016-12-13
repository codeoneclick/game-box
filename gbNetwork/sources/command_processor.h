//
//  command_processor.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "asio.hpp"
#include "net_declarations.h"

namespace gb
{
    namespace net
    {
        class command_processor
        {
        public:
            
            typedef std::function<command_shared_ptr(asio::streambuf&& buffer)> command_creator_t;
            
        private:
            
        protected:
            
            static std::map<i32, command_creator_t> m_command_creators;
            
        public:
            
            command_processor();
            ~command_processor();
            
            static command_shared_ptr deserialize(i32 command_id, asio::streambuf&& buffer);
            
            void register_command_creator(i32 command_id, const command_creator_t& creator);
        };
    };
};

