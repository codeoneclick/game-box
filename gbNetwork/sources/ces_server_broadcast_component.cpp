//
//  ces_server_broadcast_component.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_server_broadcast_component.h"
#include "asio.hpp"

namespace gb
{
    namespace net
    {
        class ces_server_broadcast_component_pimpl
        {
        private:
            
            asio::io_service m_io_service;
            asio::ip::udp::socket m_socket;
            
        protected:
            
        public:
            
            ces_server_broadcast_component_pimpl() :
            m_socket(m_io_service, asio::ip::udp::endpoint(asio::ip::udp::v4(), 0))
            {
                m_socket.set_option(asio::socket_base::broadcast(true));
            }
            
            ~ces_server_broadcast_component_pimpl()
            {
                
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
        
        ces_server_broadcast_component::ces_server_broadcast_component() :
        m_pimpl(std::make_shared<ces_server_broadcast_component_pimpl>())
        {
            
        }
        
        ces_server_broadcast_component::~ces_server_broadcast_component()
        {
            
        }
        
        void ces_server_broadcast_component::start()
        {
            m_thread = std::thread(&ces_server_broadcast_component::broadcast, this);
        }
        
        void ces_server_broadcast_component::stop()
        {
            
        }
        
        void ces_server_broadcast_component::broadcast()
        {
#if defined(__IOS__) || defined(__OSX__)

            pthread_setname_np("gb.core.server.broadcast");

#endif

            asio::ip::udp::endpoint broadcast_endpoint(asio::ip::address_v4::broadcast(), 6969);
            std::array<char, 1> buffer;
            ces_net_log_component_extension::log("broadcast server started");
			ces_net_log_component_extension::log("ip:");
			ces_net_log_component_extension::log(broadcast_endpoint.address().to_string());
			ces_net_log_component_extension::log("port:");
			ces_net_log_component_extension::log("6969");
            
            while(true)
            {
                ces_net_log_component_extension::log("broadcast message sent");
                m_pimpl->get_socket().send_to(asio::buffer(buffer), broadcast_endpoint);
                std::this_thread::sleep_for(std::chrono::milliseconds(3000));
            }
        }
    }
}
