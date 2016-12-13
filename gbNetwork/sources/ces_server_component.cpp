//
//  ces_server_component.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/9/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_server_component.h"
#include "connection.h"

namespace gb
{
    namespace net
    {
        class ces_server_component_pimpl
        {
        private:
            
            asio::io_service m_io_service;
            asio::ip::tcp::acceptor m_acceptor;
            
        protected:
            
        public:
            
            ces_server_component_pimpl() :
            m_acceptor(m_io_service, asio::ip::tcp::endpoint(asio::ip::address::from_string("127.0.0.1"), 6868))
            {
                
            }
            
            ~ces_server_component_pimpl()
            {
                
            }
            
            asio::io_service& get_io_service()
            {
                return m_io_service;
            };
            
            
            asio::ip::tcp::acceptor& get_acceptor()
            {
                return m_acceptor;
            };
        };
        
        ces_server_component::ces_server_component() :
        m_pimpl(std::make_shared<ces_server_component_pimpl>())
        {
            
        }
        
        ces_server_component::~ces_server_component()
        {
            
        }
        
        void ces_server_component::start()
        {
             m_thread = std::thread(&ces_server_component::listen_connections, this);
        }
        
        void ces_server_component::stop()
        {
            
        }
        
        void ces_server_component::listen_connections()
        {
            pthread_setname_np("gb.core.server");
            
            while(true)
            {
                connection_shared_ptr connection = std::make_shared<gb::net::connection>(m_pimpl->get_io_service());
                m_connections.push_back(connection);
                std::error_code ec;
                m_pimpl->get_acceptor().accept(connection->get_socket(), ec);
                connection->start();
                std::cerr<<"client connected with error: "<<ec<<std::endl;
            }
        }
        
        void ces_server_component::accept_connection(const std::error_code& ec)
        {
            const auto& connection = m_connections.back();
            connection->start();
            ces_server_component::listen_connections();
        }
    }
}
