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
            static const ui32 k_command_character_spawn;
            static const ui32 k_command_client_character_move;
            static const ui32 k_command_server_character_move;
            
        private:
            
            std::shared_ptr<command_pimpl> m_pimpl;
            
        protected:
            
            std::streambuf& get_buffer();
            ui32 m_command_id;
            
        public:
            
            command();
            virtual ~command();
            
            virtual std::streambuf& serialize() = 0;
            virtual void deserialize(std::streambuf&& buffer, i32 size) = 0;
            
            ui32 get_command_id() const;
        };
    };
};
