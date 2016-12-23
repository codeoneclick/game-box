//
//  ces_client_broadcast_component.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_client_broadcast_component.h"
#include "asio.hpp"

namespace gb
{
    namespace net
    {
        class ces_client_broadcast_component_pimpl
        {
        private:
            
            asio::io_service m_io_service;
            asio::ip::udp::socket m_socket;
            
        protected:
            
        public:
            
            ces_client_broadcast_component_pimpl() :
            m_socket(m_io_service, asio::ip::udp::endpoint(asio::ip::udp::v4(), 6969))
            {

            }
            
            ~ces_client_broadcast_component_pimpl()
            {
                m_socket.close();
            }
            
            asio::io_service& get_io_service()
            {
                return m_io_service;
            };
            
            
            asio::ip::udp::socket& get_socket()
            {
                return m_socket;
            };
        };
        
        ces_client_broadcast_component::ces_client_broadcast_component() :
        m_pimpl(std::make_shared<ces_client_broadcast_component_pimpl>()),
        m_is_working(false)
        {
            
        }
        
        ces_client_broadcast_component::~ces_client_broadcast_component()
        {
            ces_client_broadcast_component::stop();
        }
        
        void ces_client_broadcast_component::start()
        {
            m_is_working = true;
            m_thread = std::thread(&ces_client_broadcast_component::update, this);
        }
        
        void ces_client_broadcast_component::stop()
        {
            m_is_working = false;
            if(m_thread.joinable())
            {
                m_thread.join();
            }
        }
        
        void ces_client_broadcast_component::update()
        {
            pthread_setname_np("gb.core.client.broadcast");

            asio::ip::udp::endpoint sender_endpoint;
            
            while(m_is_working)
            {
                std::array<char, 1> buffer;
                m_pimpl->get_socket().receive_from(asio::buffer(buffer), sender_endpoint);
                ces_net_endpoint_component_extension::add_endpoint(sender_endpoint.address().to_string());
            }
        }
    }
}
