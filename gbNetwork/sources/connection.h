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
        class connection_pimpl;
        class connection
        {
        private:
            
            std::shared_ptr<connection_pimpl> m_pimpl;
            std::thread m_thread_io;
            
            std::queue<command_shared_ptr> m_commands_to_send;
            std::queue<command_shared_ptr> m_commands_to_receive;
            mutable std::recursive_mutex m_command_sending_mutex;
            mutable std::recursive_mutex m_command_receiving_mutex;
            
        protected:
            
            void read_header();
            void read_body(i32 command_id, i32 command_size);
            
            void run();
            
        public:
            
            connection();
            ~connection();
            
            asio::ip::tcp::socket& get_socket();
            asio::io_service& get_io_service();
            
            std::error_code establish(const std::string& ip, i32 port);
            void start();
            
            void send_command(command_const_shared_ptr command);
            
            bool is_received_commands_exist() const;
            std::queue<command_shared_ptr> get_received_commands();
            
            void update();
            
            bool is_closed();
        };
    };
};
