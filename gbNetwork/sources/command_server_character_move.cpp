//
//  command_server_character_move.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "command_server_character_move.h"

namespace gb
{
    namespace net
    {
        command_server_character_move::command_server_character_move()
        {
            m_command_id = command::k_command_server_character_move;
            m_udid = std::numeric_limits<ui32>::max();
            m_velocity = glm::vec2(0.f);
            m_position = glm::vec2(0.f);
            m_rotation = 0.f;
            m_is_moving = false;
        }
        
        command_server_character_move::command_server_character_move(ui32 udid,
                                                                     const glm::vec2& velocity, const glm::vec2& position,
                                                                     f32 rotation, bool is_moving)
        {
            m_udid = udid;
            m_velocity = velocity;
            m_position = position;
            m_rotation = rotation;
            m_is_moving = is_moving;
        }
        
        command_server_character_move::~command_server_character_move()
        {
            
        }
        
        command_server_character_move_shared_ptr command_server_character_move::create(std::streambuf&& buffer, i32 size)
        {
            command_server_character_move_shared_ptr command = std::make_shared<command_server_character_move>();
            command->deserialize(std::move(buffer), size);
            return command;
        }
        
        std::streambuf& command_server_character_move::serialize()
        {
            std::ostream stream(&command::get_buffer());
            i32 id = command_server_character_move::get_command_id();
            stream.write((const char*)&id, sizeof(id));
            i32 size = sizeof(m_udid);
            size += sizeof(m_velocity);
            size += sizeof(m_position);
            size += sizeof(m_rotation);
            size += sizeof(m_is_moving);
            stream.write((const char*)&size, sizeof(size));
            stream.write((const char*)&m_udid, sizeof(m_udid));
            stream.write((const char*)&m_velocity, sizeof(m_velocity));
            stream.write((const char*)&m_position, sizeof(m_position));
            stream.write((const char*)&m_rotation, sizeof(m_rotation));
            stream.write((const char*)&m_is_moving, sizeof(m_is_moving));
            return command::get_buffer();
        }
        
        void command_server_character_move::deserialize(std::streambuf&& buffer, i32 size)
        {
            std::istream stream(&buffer);
            stream.read((char *)&m_udid, sizeof(m_udid));
            stream.read((char *)&m_velocity, sizeof(m_velocity));
            stream.read((char *)&m_position, sizeof(m_position));
            stream.read((char *)&m_rotation, sizeof(m_rotation));
            stream.read((char *)&m_is_moving, sizeof(m_is_moving));
        }
        
        ui32 command_server_character_move::get_udid() const
        {
            return m_udid;
        }
        
        glm::vec2 command_server_character_move::get_velocity() const
        {
            return m_velocity;
        }
        
        glm::vec2 command_server_character_move::get_position() const
        {
            return m_position;
        }
        
        f32 command_server_character_move::get_rotation() const
        {
            return m_rotation;
        }
        
        bool command_server_character_move::is_moving() const
        {
            return m_is_moving;
        }
    }
}
