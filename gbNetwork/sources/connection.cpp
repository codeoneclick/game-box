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
        connection::connection(asio::io_service& io_service) :
        m_io_service(io_service),
        m_socket(io_service),
        m_is_closed(false)
        {
            
        }
        
        connection::~connection()
        {
            m_thread_sending.join();
            m_thread_receiving.join();
        }
        
        void connection::run_receiving()
        {
#if defined(__IOS__) || defined(__OSX__)

            pthread_setname_np("gb.core.connection.run_receiving");

#endif
            while(!m_is_closed)
            {
                asio::streambuf header;
                std::error_code ec;
                asio::read(m_socket, header, asio::transfer_exactly(command::k_header_size), ec);
                if(!ec)
                {
                    i32 command_id = -1;
                    i32 command_size = -1;
                    std::istream stream(&header);
                    stream.read((char *)&command_id, sizeof(command_id));
                    stream.read((char *)&command_size, sizeof(command_size));
                    header.consume(command::k_header_size);
                    
                    asio::streambuf data;
                    asio::read(m_socket, data, asio::transfer_exactly(command_size), ec);
                    assert(!ec);
                    command_shared_ptr command = command_processor::deserialize(command_id, std::move(data), command_size);
                    data.consume(command_size);
                    
                    std::lock_guard<std::recursive_mutex> guard(m_command_receiving_mutex);
                    m_commands_to_receive.push(command);
                }
                else
                {
                    m_socket.close();
                    m_is_closed = true;
                    std::cerr<<"socket closed: "<<ec<<std::endl;
                }
                //std::this_thread::yield();
            }
        }
        
        void connection::run_sending()
        {
#if defined(__IOS__) || defined(__OSX__)
            
            pthread_setname_np("gb.core.connection.run_sending");
            
#endif
            while(!m_is_closed)
            {
                std::lock_guard<std::recursive_mutex> guard(m_command_sending_mutex);
                while (!m_commands_to_send.empty())
                {
                    command_shared_ptr command = m_commands_to_send.front();
                    asio::streambuf& buffer = static_cast<asio::streambuf&>(command->serialize());
                    asio::write(m_socket, buffer);
                    m_commands_to_send.pop();
                }
                //std::this_thread::yield();
            }
        }
        
        std::error_code connection::establish(const std::string& ip, i32 port)
        {
            std::stringstream port_str;
            port_str<<port;
            asio::ip::tcp::resolver resolver(m_io_service);
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
            m_thread_receiving = std::thread(&connection::run_receiving, this);
            m_thread_sending = std::thread(&connection::run_sending, this);
        }
        
        asio::ip::tcp::socket& connection::get_socket()
        {
            return m_socket;
        }
        
        asio::io_service& connection::get_io_service()
        {
            return m_io_service;
        }
        
        bool connection::is_closed() const
        {
            return m_is_closed;
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
            return std::move(commands_to_receive);
        }
    }
}
