//
//  command_server_character_shoot.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 4/6/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "command_server_character_shoot.h"
namespace gb
{
    namespace net
    {
        command_server_character_shoot::command_server_character_shoot()
        {
            command_server_character_shoot::init();
            m_shoot_revision = std::numeric_limits<ui64>::max();
            m_udid = -1;
            m_velocity = glm::vec2(0.f);
            m_position = glm::vec2(0.f);
            m_rotation = 0.f;
        }
        
        command_server_character_shoot::command_server_character_shoot(ui64 shoot_revision,
                                                                       i32 udid,
                                                                       const glm::vec2& velocity,
                                                                       const glm::vec2& position,
                                                                       f32 rotation)
        {
            command_server_character_shoot::init();
            m_shoot_revision = shoot_revision;
            m_udid = udid;
            m_velocity = velocity;
            m_position = position;
            m_rotation = rotation;
        }
        
        command_server_character_shoot::~command_server_character_shoot()
        {
            
        }
        
        command_server_character_shoot_shared_ptr command_server_character_shoot::create(const std::shared_ptr<std::streambuf>& buffer)
        {
            command_server_character_shoot_shared_ptr command = std::make_shared<command_server_character_shoot>();
            command->deserialize(buffer);
            return command;
        }
        
        void command_server_character_shoot::init()
        {
            command_id.getter([=] {
                return command::k_command_server_character_shoot;
            });
            description.getter([=] {
                std::stringstream str_stream;
                str_stream<<"shoot_revision: "<<m_shoot_revision<<"; ";
                str_stream<<"udid: "<<m_udid<<"; ";
                str_stream<<"velocity_x: "<<m_velocity.x<<"; ";
                str_stream<<"velocity_y: "<<m_velocity.y<<"; ";
                str_stream<<"position_x: "<<m_position.x<<"; ";
                str_stream<<"position_y: "<<m_position.y<<"; ";
                str_stream<<"rotation: "<<m_rotation<<"; ";
                return str_stream.str();
            });
            
            shoot_revision.getter([=] {
                return m_shoot_revision;
            });
            
            udid.getter([=] {
                return m_udid;
            });
            
            velocity.getter([=] {
                return m_velocity;
            });
            
            position.getter([=] {
                return m_position;
            });
            
            rotation.getter([=] {
                return m_rotation;
            });
        }
        
        std::shared_ptr<std::streambuf> command_server_character_shoot::serialize()
        {
            std::shared_ptr<std::streambuf> buffer = command::buffer;
            std::ostream stream(buffer.get());
            i32 id = command::k_command_server_character_shoot;
            stream.write((const char*)&id, sizeof(id));
            i32 size = sizeof(m_shoot_revision);
            size += sizeof(m_udid);
            size += sizeof(m_velocity);
            size += sizeof(m_position);
            size += sizeof(m_rotation);
            stream.write((const char*)&size, sizeof(size));
            stream.write((const char*)&m_shoot_revision, sizeof(m_shoot_revision));
            stream.write((const char*)&m_udid, sizeof(m_udid));
            stream.write((const char*)&m_velocity, sizeof(m_velocity));
            stream.write((const char*)&m_position, sizeof(m_position));
            stream.write((const char*)&m_rotation, sizeof(m_rotation));
            return buffer;
        }
        
        void command_server_character_shoot::deserialize(const std::shared_ptr<std::streambuf>& buffer)
        {
            std::istream stream(buffer.get());
            stream.read((char *)&m_shoot_revision, sizeof(m_shoot_revision));
            stream.read((char *)&m_udid, sizeof(m_udid));
            stream.read((char *)&m_velocity, sizeof(m_velocity));
            stream.read((char *)&m_position, sizeof(m_position));
            stream.read((char *)&m_rotation, sizeof(m_rotation));
        }
    }
}
