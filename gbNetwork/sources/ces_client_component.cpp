//
//  ces_client_component.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/10/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_client_component.h"
#include "connection.h"

namespace gb
{
    namespace net
    {
        class ces_client_component_pimpl
        {
        private:
            
            asio::io_service m_io_service;
            
        protected:
            
        public:
            
            ces_client_component_pimpl()
            {
                
            }
            
            ~ces_client_component_pimpl()
            {
                
            }
            
            asio::io_service& get_io_service()
            {
                return m_io_service;
            };
        };
        
        ces_client_component::ces_client_component() :
        m_pimpl(std::make_shared<ces_client_component_pimpl>())
        {
            m_connection = std::make_shared<gb::net::connection>(m_pimpl->get_io_service());
        }
        
        ces_client_component::~ces_client_component()
        {
            
        }
        
        void ces_client_component::connect()
        {
            std::cerr<<"connect error: "<<m_connection->establish()<<std::endl;
            m_connection->start();
        }
        
        void ces_client_component::disconnect()
        {
            m_connection->get_socket().close();
        }
        
        connection_shared_ptr ces_client_component::get_connection() const
        {
            return m_connection;
        }
    }
}