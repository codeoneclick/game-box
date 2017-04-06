//
//  command_client_character_shoot.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 4/6/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "command_client_character_shoot.h"

namespace gb
{
    namespace net
    {
        command_client_character_shoot::command_client_character_shoot()
        {
            command_client_character_shoot::init();
            m_shoot_revision = std::numeric_limits<ui64>::max();
            m_udid = -1;
            m_shoot_angle = 0.f;
        }
        
        command_client_character_shoot::command_client_character_shoot(ui64 shoot_revision,
                                                                       i32 udid,
                                                                       f32 shoot_angle)
        {
            command_client_character_shoot::init();
            m_shoot_revision = shoot_revision;
            m_udid = udid;
            m_shoot_angle = shoot_angle;
        }
        
        command_client_character_shoot::~command_client_character_shoot()
        {
            
        }
        
        command_client_character_shoot_shared_ptr command_client_character_shoot::create(const std::shared_ptr<std::streambuf>& buffer)
        {
            command_client_character_shoot_shared_ptr command = std::make_shared<command_client_character_shoot>();
            command->deserialize(buffer);
            return command;
        }
        
        void command_client_character_shoot::init()
        {
            command_id.getter([=] {
                return command::k_command_client_character_shoot;
            });
            description.getter([=] {
                std::stringstream str_stream;
                str_stream<<"shoot_revision: "<<m_shoot_revision<<"; ";
                str_stream<<"udid: "<<m_udid<<"; ";
                str_stream<<"shoot_angle: "<<m_shoot_angle<<"; ";
                return str_stream.str();
            });
            
            shoot_revision.getter([=] {
                return m_shoot_revision;
            });
            
            udid.getter([=] {
                return m_udid;
            });
            
            shoot_angle.getter([=] {
                return m_shoot_angle;
            });
        }
        
        std::shared_ptr<std::streambuf> command_client_character_shoot::serialize()
        {
            std::shared_ptr<std::streambuf> buffer = command::buffer;
            std::ostream stream(buffer.get());
            i32 id = command::k_command_client_character_shoot;
            stream.write((const char*)&id, sizeof(id));
            i32 size = sizeof(m_shoot_revision);
            size += sizeof(m_udid);
            size += sizeof(m_shoot_angle);
            stream.write((const char*)&size, sizeof(size));
            stream.write((const char*)&m_shoot_revision, sizeof(m_shoot_revision));
            stream.write((const char*)&m_udid, sizeof(m_udid));
            stream.write((const char*)&m_shoot_angle, sizeof(m_shoot_angle));
            return buffer;
        }
        
        void command_client_character_shoot::deserialize(const std::shared_ptr<std::streambuf>& buffer)
        {
            std::istream stream(buffer.get());
            stream.read((char *)&m_shoot_revision, sizeof(m_shoot_revision));
            stream.read((char *)&m_udid, sizeof(m_udid));
            stream.read((char *)&m_shoot_angle, sizeof(m_shoot_angle));
        }
    }
}
