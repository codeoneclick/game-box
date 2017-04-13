//
//  ping_connection.hpp
//  gbNetwork
//
//  Created by serhii serhiiv on 4/11/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "asio.hpp"
#include "net_declarations.h"

namespace gb
{
    namespace net
    {
        class ping_connection_pimpl;
        class ping_connection
        {
        public:
            
            typedef std::function<void(const std::error_code&)> ping_callback_t;
            
        private:
            
            std::shared_ptr<ping_connection_pimpl> m_pimpl;
            std::thread m_thread_io;
            asio::ip::icmp::socket m_socket;
            ping_callback_t m_ping_callback;
            bool m_is_ended;
            
        protected:
            
            void receive(std::size_t length);
            void timeout();
            
            void start();
            void run();
            
        public:
            
            ping_connection(asio::io_service& io_service);
            ~ping_connection();
            
            void establish(const std::string& ip, i32 port);
            void set_ping_callback(const ping_callback_t& callback);
        };
    };
};

