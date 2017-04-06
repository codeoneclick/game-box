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
            
            std::shared_ptr<asio::streambuf> m_buffer;
            
        protected:
            
        public:
            
            command_pimpl() :
            m_buffer(std::make_shared<asio::streambuf>())
            {
                buffer.getter([=]() {
                    return m_buffer;
                });
            }
            
            ~command_pimpl()
            {
                
            }
            std::property_ro<std::shared_ptr<asio::streambuf>> buffer;
        };

        
        const i32 command::k_header_size = 8;
        
        const ui32 command::k_command_client_connection_established = 1;
        const ui32 command::k_command_character_spawn = 2;
        const ui32 command::k_command_client_character_move = 3;
        const ui32 command::k_command_server_character_move = 4;
        const ui32 command::k_command_client_character_shoot = 5;
        const ui32 command::k_command_server_character_shoot = 6;
        
        command::command() :
        m_pimpl(std::make_shared<command_pimpl>())
        {
            command::init();
            
            buffer.getter([=]() {
                std::shared_ptr<asio::streambuf> buffer = m_pimpl->buffer;
                return buffer;
            });
        }
        
        command::~command()
        {
            
        }
        
        void command::init()
        {
            command_id.getter([=] {
                return std::numeric_limits<ui32>::max();
            });
            description.getter([=] {
                return "unknown";
            });
        }
    }
}
