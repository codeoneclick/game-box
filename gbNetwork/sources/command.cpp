//
//  command.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "command.h"
#include "asio.hpp"

namespace gb
{
    namespace net
    {
        
        class command_pimpl
        {
        private:
            
            asio::streambuf m_buffer;
            
        protected:
            
        public:
            
            command_pimpl()
            {
                
            }
            
            ~command_pimpl()
            {
                
            }
            
            asio::streambuf& get_buffer()
            {
                return m_buffer;
            };
        };

        
        const i32 command::k_header_size = 8;
        
        const ui32 command::k_command_client_connection_established = 1;
        const ui32 command::k_command_character_spawn = 2;
        const ui32 command::k_command_client_character_move = 3;
        const ui32 command::k_command_server_character_move = 4;
        
        command::command() :
        m_pimpl(std::make_shared<command_pimpl>())
        {
            m_command_id = std::numeric_limits<ui32>::max();
        }
        
        command::~command()
        {
            
        }
        
        std::streambuf& command::get_buffer()
        {
            return m_pimpl->get_buffer();
        }
        
        ui32 command::get_command_id() const
        {
            return m_command_id;
        }
    }
}
