//
//  command.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "net_declarations.h"
#include "std_extensions.h"

namespace gb
{
    namespace net
    {
        class command_pimpl;
        class command
        {
        public:
            
            static const i32 k_header_size;
            
            static const ui32 k_command_client_connection_established;
            static const ui32 k_command_client_join;
            static const ui32 k_command_character_spawn;
            static const ui32 k_command_client_character_move;
            static const ui32 k_command_server_character_move;
            static const ui32 k_command_client_character_shoot;
            static const ui32 k_command_server_character_shoot;
            
        private:
            
            std::shared_ptr<command_pimpl> m_pimpl;
            
        protected:
            
            std::property_ro<std::shared_ptr<std::streambuf>> buffer;
            
        public:
            
            command();
            virtual ~command();
            
            virtual void init();
            
            virtual std::shared_ptr<std::streambuf> serialize() = 0;
            virtual void deserialize(const std::shared_ptr<std::streambuf>& buffer) = 0;
            
            std::property_ro<std::string> description;
            std::property_ro<ui32> command_id;
        };
    };
};
