//
//  command_character_spawn.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 1/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "command_character_spawn.h"

namespace gb
{
    namespace net
    {
        command_character_spawn::command_character_spawn()
        {
            command_character_spawn::init();
            m_udid = -1;
            m_position = glm::vec2(0.f);
            m_rotation = 0.f;
        }
        
        command_character_spawn::command_character_spawn(i32 udid, const glm::vec2& position, f32 rotation)
        {
            command_character_spawn::init();
            m_udid = udid;
            m_position = position;
            m_rotation = rotation;
        }
        
        command_character_spawn::~command_character_spawn()
        {
            
        }
        
        command_character_spawn_shared_ptr command_character_spawn::create(std::streambuf&& buffer, i32 size)
        {
            command_character_spawn_shared_ptr command = std::make_shared<command_character_spawn>();
            command->deserialize(std::move(buffer), size);
            return command;
        }
        
        void command_character_spawn::init()
        {
            command_id.getter([=] {
                return command::k_command_character_spawn;
            });
            description.getter([=] {
                std::stringstream str_stream;
                str_stream<<"udid: "<<m_udid<<"; ";
                str_stream<<"position_x: "<<m_position.x<<"; ";
                str_stream<<"position_y: "<<m_position.y<<"; ";
                str_stream<<"rotation: "<<m_rotation<<";";
                return str_stream.str();
            });
            
            udid.getter([=] {
                return m_udid;
            });
            
            position.getter([=] {
                return m_position;
            });
            
            rotation.getter([=] {
                return m_rotation;
            });
        }
        
        std::streambuf& command_character_spawn::serialize()
        {
            std::ostream stream(&command::get_buffer());
            i32 id = command::k_command_character_spawn;
            stream.write((const char*)&id, sizeof(id));
            
            i32 size = sizeof(m_udid);
            size += sizeof(m_position);
            size += sizeof(m_rotation);
            
            stream.write((const char*)&size, sizeof(size));
            stream.write((const char*)&m_udid, sizeof(m_udid));
            stream.write((const char*)&m_position, sizeof(m_position));
            stream.write((const char*)&m_rotation, sizeof(m_rotation));
            
            return command::get_buffer();
        }
        
        void command_character_spawn::deserialize(std::streambuf&& buffer, i32 size)
        {
            std::istream stream(&buffer);
            stream.read((char *)&m_udid, sizeof(m_udid));
            stream.read((char *)&m_position, sizeof(m_position));
            stream.read((char *)&m_rotation, sizeof(m_rotation));
        }
    }
}
