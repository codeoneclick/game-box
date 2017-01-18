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

        
        std::set<uintptr_t> command::g_guids_container;
        const i32 command::k_header_size = 8;
        
        command::command() :
        m_pimpl(std::make_shared<command_pimpl>())
        {
            
        }
        
        command::~command()
        {
            
        }
        
        std::streambuf& command::get_buffer()
        {
            return m_pimpl->get_buffer();
        }
    }
}
