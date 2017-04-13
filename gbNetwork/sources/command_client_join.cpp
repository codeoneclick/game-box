//
//  command_client_join.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 4/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "command_client_join.h"

namespace gb
{
    namespace net
    {
        command_client_join::command_client_join()
        {
            command_client_join::init();
            m_udid = -1;
        }
        
        command_client_join::command_client_join(i32 udid)
        {
            command_client_join::init();
            m_udid = udid;
        }
        
        command_client_join::~command_client_join()
        {
            
        }
        
        command_client_join_shared_ptr command_client_join::create(const std::shared_ptr<std::streambuf>& buffer)
        {
            command_client_join_shared_ptr command = std::make_shared<command_client_join>();
            command->deserialize(buffer);
            return command;
        }
        
        void command_client_join::init()
        {
            command_id.getter([=] {
                return command::k_command_client_join;
            });
            description.getter([=] {
                std::stringstream str_stream;
                str_stream<<"udid: "<<m_udid<<";";
                return str_stream.str();
            });
            
            udid.getter([=] {
                return m_udid;
            });
        }
        
        std::shared_ptr<std::streambuf> command_client_join::serialize()
        {
            std::shared_ptr<std::streambuf> buffer = command::buffer;
            std::ostream stream(buffer.get());
            i32 id = command::k_command_client_join;
            stream.write((const char*)&id, sizeof(id));
            
            i32 size = sizeof(m_udid);
            
            stream.write((const char*)&size, sizeof(size));
            stream.write((const char*)&m_udid, sizeof(m_udid));
            
            return buffer;
        }
        
        void command_client_join::deserialize(const std::shared_ptr<std::streambuf>& buffer)
        {
            std::istream stream(buffer.get());
            stream.read((char *)&m_udid, sizeof(m_udid));
        }
    }
}
