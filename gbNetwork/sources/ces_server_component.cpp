//
//  ces_server_component.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/9/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_server_component.h"
#include "connection.h"
#include "std_extensions.h"
#include "thread_operation.h"

namespace gb
{
    namespace net
    {
        i32 ces_server_component::g_connection_udid = 1;
        
        class ces_server_component_pimpl
        {
        private:
            
            asio::io_service m_io_service;
            asio::ip::tcp::acceptor m_acceptor;
            
        protected:
            
        public:
            
            ces_server_component_pimpl() :
            m_acceptor(m_io_service, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), 6868))
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
        m_pimpl(std::make_shared<ces_server_component_pimpl>()),
        m_connection_established_callback(nullptr),
        m_connection_closed_callback(nullptr)
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
        
        void ces_server_component::update_connection_status()
        {
            std::erase_if(m_connections, [=](std::pair<ui32, connection_shared_ptr> iterator) {
                if(iterator.second->is_closed())
                {
                    std::cout<<"connection closed"<<std::endl;
                    if(m_connection_closed_callback)
                    {
                        m_connection_closed_callback(iterator.first);
                    }
                }
                else
                {
                    iterator.second->update();
                }
                return iterator.second->is_closed();
            });
        }
        
        void ces_server_component::listen_connections()
        {
#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)

            pthread_setname_np("gb.core.server");

#endif
            
            ces_net_log_component_extension::log("game server listen connections");
			ces_net_log_component_extension::log("ip: ");
			ces_net_log_component_extension::log(m_pimpl->get_acceptor().local_endpoint().address().to_string());
			ces_net_log_component_extension::log("port:");
			ces_net_log_component_extension::log("6868");
            
            while(true)
            {
                connection_shared_ptr connection = std::make_shared<gb::net::connection>(m_pimpl->get_io_service());
                
                std::error_code ec;
                m_pimpl->get_acceptor().accept(connection->get_socket(), ec);
                connection->start();
                
				{
					std::lock_guard<std::recursive_mutex> guard(m_connections_mutex);
					m_connections.insert(std::make_pair(g_connection_udid, connection));
				}

                g_connection_udid++;
                
                std::cerr<<"client connected with status: "<<ec<<std::endl;
                std::cout<<"client ip: "<<connection->get_socket().remote_endpoint().address().to_string()<<std::endl;
                std::cout<<"port: "<<connection->get_socket().remote_endpoint().port()<<std::endl;
                
                if(m_connection_established_callback)
                {
                    ui32 current_connection_udid = g_connection_udid - 1;
                    gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
                    operation->set_execution_callback([=]() {
                        m_connection_established_callback(current_connection_udid);
                    });
                    operation->add_to_execution_queue();
                }
            }
        }
        
        void ces_server_component::send_command(command_const_shared_ptr command, i32 udid)
        {
            std::lock_guard<std::recursive_mutex> guard(m_connections_mutex);
            if(udid != -1)
            {
                auto iterator = m_connections.find(udid);
                if(iterator != m_connections.end())
                {
                    iterator->second->send_command(command);
                }
            }
            else
            {
                for(const auto& connection : m_connections)
                {
                    connection.second->send_command(command);
                }
            }
        }
    
        std::map<i32, connection_shared_ptr> ces_server_component::get_connections() const
        {
            std::lock_guard<std::recursive_mutex> guard(m_connections_mutex);
            return m_connections;
        }
        
        connection_shared_ptr ces_server_component::get_connection(i32 udid) const
        {
            std::lock_guard<std::recursive_mutex> guard(m_connections_mutex);
            connection_shared_ptr connection = nullptr;
            auto iterator = m_connections.find(udid);
            if(iterator != m_connections.end())
            {
                connection = iterator->second;
            }
            return connection;
        }
        
        void ces_server_component::set_connection_established_callback(const connection_established_callback_t& callback)
        {
            m_connection_established_callback = callback;
        }
        
        void ces_server_component::set_connection_closed_callback(const connection_closed_callback_t& callback)
        {
            m_connection_closed_callback = callback;
        }
    }
}
