//
//  connection.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/9/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "connection.h"
#include "command.h"
#include "command_processor.h"

namespace gb
{
    namespace net
    {
        class connection_pimpl
        {
        private:
            
            asio::io_service& m_io_service;
            asio::streambuf m_command_header_buffer;
            asio::streambuf m_command_body_buffer;
            
        protected:
            
        public:
            
            connection_pimpl(asio::io_service& io_service) :
            m_io_service(io_service)
            {
                
            }
            
            ~connection_pimpl()
            {
                
            }
            
            asio::streambuf& get_command_header_buffer()
            {
                return m_command_header_buffer;
            };
            
            asio::streambuf& get_command_body_buffer()
            {
                return m_command_body_buffer;
            };
            
            asio::io_service& get_io_service()
            {
                return m_io_service;
            };
        };
        
        connection::connection(asio::io_service& io_service) :
        m_socket(io_service),
        m_pimpl(std::make_shared<connection_pimpl>(io_service))
        {
            
        }
        
        connection::~connection()
        {
            m_socket.close();
            m_thread_io.join();
        }
        
        void connection::read_header()
        {
            std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
            std::error_code ec;
            asio::async_read(m_socket, (*buffer.get()), asio::transfer_exactly(command::k_header_size), [this, buffer] (const std::error_code& ec, std::size_t length) {
                if(!ec)
                {
                    std::cout<<"async_read header: "<<length<<" bytes"<<std::endl;
                    i32 command_id = -1;
                    i32 command_size = -1;
                    std::istream stream(&(*buffer.get()));
                    stream.read((char *)&command_id, sizeof(command_id));
                    stream.read((char *)&command_size, sizeof(command_size));
                    connection::read_body(command_id, command_size);
                }
                else
                {
                    m_socket.get_io_service().stop();
                    m_socket.close();
                    std::cerr<<"socket closed: "<<ec<<std::endl;
                }
            });
        }
        
        void connection::read_body(i32 command_id, i32 command_size)
        {
            std::shared_ptr<asio::streambuf> buffer = std::make_shared<asio::streambuf>();
            asio::async_read(m_socket, (*buffer.get()), asio::transfer_exactly(command_size), [this, command_id, command_size, buffer] (const std::error_code& ec, std::size_t length) {
                
                if(!ec)
                {
                    std::cout<<"async_read body: "<<length<<" bytes"<<std::endl;
                    command_shared_ptr command = command_processor::deserialize(command_id, buffer);
                    {
                        std::lock_guard<std::recursive_mutex> guard(m_command_receiving_mutex);
                        m_commands_to_receive.push(command);
                    }
                    connection::read_header();
                }
                else
                {
                    m_socket.get_io_service().stop();
                    m_socket.close();
                    std::cerr<<"socket closed: "<<ec<<std::endl;
                }
            });
        }
        
        void connection::update()
        {
            std::lock_guard<std::recursive_mutex> guard(m_command_sending_mutex);
            while(!m_commands_to_send.empty())
            {
                command_shared_ptr command = m_commands_to_send.front();
                std::shared_ptr<asio::streambuf> buffer = std::static_pointer_cast<asio::streambuf>(command->serialize());
                asio::async_write(m_socket, (*buffer.get()), [this, buffer] (const std::error_code& ec, std::size_t length) {
                    if(!ec)
                    {
                        std::cout<<"async_write: "<<length<<" bytes"<<std::endl;
                    }
                    else
                    {
                        m_socket.get_io_service().stop();
                        m_socket.close();
                        std::cerr<<"socket closed: "<<ec<<std::endl;
                    }
                });
                m_commands_to_send.pop();
            }
        }
        
        void connection::run()
        {
#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)
            
            pthread_setname_np("gb.core.connection.run");
            
#endif
            
            while(m_socket.is_open())
            {
                std::error_code ec;
                asio::io_service::work work(m_socket.get_io_service());
                m_socket.get_io_service().run(ec);
                assert(!ec);
            }
        }
        
        std::error_code connection::establish(const std::string& ip, i32 port)
        {
            std::stringstream port_str;
            port_str<<port;
            asio::ip::tcp::resolver resolver(m_pimpl->get_io_service());
            asio::ip::tcp::resolver::query query(ip, port_str.str());
            asio::ip::tcp::resolver::iterator endpoint_iterator = resolver.resolve(query);
            asio::ip::tcp::resolver::iterator end;
            
            std::error_code ec;
            while(endpoint_iterator != end)
            {
                m_socket.close();
                m_socket.connect(*endpoint_iterator++, ec);
            }
            return ec;
        }
        
        void connection::send_command(command_const_shared_ptr command)
        {
            std::lock_guard<std::recursive_mutex> guard(m_command_sending_mutex);
            m_commands_to_send.push(command);
        }
        
        void connection::start()
        {
            m_thread_io = std::thread(&connection::run, this);
            connection::read_header();
        }
        
        asio::ip::tcp::socket& connection::get_socket()
        {
            return m_socket;
        }
        
        asio::io_service& connection::get_io_service()
        {
            return m_pimpl->get_io_service();
        }
        
        bool connection::is_closed() const
        {
            return !m_socket.is_open();
        }
        
        bool connection::is_received_commands_exist() const
        {
            std::lock_guard<std::recursive_mutex> guard(m_command_receiving_mutex);
            return !m_commands_to_receive.empty();
        }
        
        std::queue<command_shared_ptr> connection::get_received_commands()
        {
            std::lock_guard<std::recursive_mutex> guard(m_command_receiving_mutex);
            std::queue<command_shared_ptr> commands_to_receive = m_commands_to_receive;
            while (!m_commands_to_receive.empty())
            {
                m_commands_to_receive.pop();
            }
            return commands_to_receive;
        }
    }
}
