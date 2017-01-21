//
//  command_client_connection_established.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 1/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "command_client_connection_established.h"

namespace gb
{
    namespace net
    {
        command_client_connection_established::command_client_connection_established()
        {
            m_command_id = command::k_command_client_connection_established;
            m_udid = std::numeric_limits<ui32>::max();
        }
        
        command_client_connection_established::command_client_connection_established(ui32 udid)
        {
            m_udid = udid;
        }
        
        command_client_connection_established::~command_client_connection_established()
        {
            
        }
        
        command_client_connection_established_shared_ptr command_client_connection_established::create(std::streambuf&& buffer, i32 size)
        {
            command_client_connection_established_shared_ptr command = std::make_shared<command_client_connection_established>();
            command->deserialize(std::move(buffer), size);
            return command;
        }
        
        std::streambuf& command_client_connection_established::serialize()
        {
            std::ostream stream(&command::get_buffer());
            i32 id = command_client_connection_established::get_command_id();
            stream.write((const char*)&id, sizeof(id));
            i32 size = sizeof(m_udid);
            stream.write((const char*)&size, sizeof(size));
            stream.write((const char*)&m_udid, sizeof(m_udid));
            return command::get_buffer();
        }
        
        void command_client_connection_established::deserialize(std::streambuf&& buffer, i32 size)
        {
            std::istream stream(&buffer);
            stream.read((char *)&m_udid, size);
        }
        
        ui32 command_client_connection_established::get_udid() const
        {
            return m_udid;
        }
    }
}
