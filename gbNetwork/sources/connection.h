//
//  connection.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/9/16.
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
        class connection
        {
        private:
            
            std::thread m_thread_sending;
            std::thread m_thread_receiving;
            
            std::queue<command_shared_ptr> m_commands_to_send;
            std::queue<command_shared_ptr> m_commands_to_receive;
            mutable std::recursive_mutex m_mutex;
            
        protected:
            
            asio::io_service& m_io_service;
            asio::ip::tcp::socket m_socket;
            
            void run_sending();
            void run_receiving();
            
        public:
            
            connection(asio::io_service& io_service);
            ~connection();
            
            asio::ip::tcp::socket& get_socket();
            asio::io_service& get_io_service();
            
            std::error_code establish();
            void start();
            
            void send_command(command_const_shared_ptr command);
        };
    };
};
