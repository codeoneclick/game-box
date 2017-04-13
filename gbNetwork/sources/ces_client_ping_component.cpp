//
//  ces_client_ping_component.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 4/11/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_client_ping_component.h"
#include "ping_connection.h"

namespace gb
{
    namespace net
    {
        class ces_client_ping_component_pimpl
        {
        private:
            
            asio::io_service m_io_service;
            
        protected:
            
        public:
            
            ces_client_ping_component_pimpl()
            {
                
            }
            
            ~ces_client_ping_component_pimpl()
            {
                
            }
            
            asio::io_service& get_io_service()
            {
                return m_io_service;
            };
        };
        
        ces_client_ping_component::ces_client_ping_component() :
        m_pimpl(std::make_shared<ces_client_ping_component_pimpl>())
        {
            m_ping_connection = std::make_shared<gb::net::ping_connection>(m_pimpl->get_io_service());
        }
        
        ces_client_ping_component::~ces_client_ping_component()
        {
            
        }
        
        void ces_client_ping_component::check(const std::string& ip, i32 port, const ping_callback_t& callback)
        {
            m_ping_connection->set_ping_callback(callback);
            m_ping_connection->establish(ip, port);
        }
    }
}
