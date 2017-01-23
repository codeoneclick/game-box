//
//  command_client_character_move.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 1/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "command_client_character_move.h"

namespace gb
{
    namespace net
    {
        command_client_character_move::command_client_character_move()
        {
            m_command_id = command::k_command_client_character_move;
            m_udid = std::numeric_limits<ui32>::max();
            m_delta = glm::vec2(0.f);
            m_is_moving = false;
        }
        
        command_client_character_move::command_client_character_move(ui32 udid, const glm::vec2& delta, bool is_moving)
        {
            m_udid = udid;
            m_delta = delta;
            m_is_moving = is_moving;
        }
        
        command_client_character_move::~command_client_character_move()
        {
            
        }
        
        command_client_character_move_shared_ptr command_client_character_move::create(std::streambuf&& buffer, i32 size)
        {
            command_client_character_move_shared_ptr command = std::make_shared<command_client_character_move>();
            command->deserialize(std::move(buffer), size);
            return command;
        }
        
        std::streambuf& command_client_character_move::serialize()
        {
            std::ostream stream(&command::get_buffer());
            i32 id = command::k_command_client_character_move;
            stream.write((const char*)&id, sizeof(id));
            i32 size = sizeof(m_udid);
            size += sizeof(m_delta);
            size += sizeof(m_is_moving);
            stream.write((const char*)&size, sizeof(size));
            stream.write((const char*)&m_udid, sizeof(m_udid));
            stream.write((const char*)&m_delta, sizeof(m_delta));
            stream.write((const char*)&m_is_moving, sizeof(m_is_moving));
            return command::get_buffer();
        }
        
        void command_client_character_move::deserialize(std::streambuf&& buffer, i32 size)
        {
            std::istream stream(&buffer);
            stream.read((char *)&m_udid, sizeof(m_udid));
            stream.read((char *)&m_delta, sizeof(m_delta));
            stream.read((char *)&m_is_moving, sizeof(m_is_moving));
        }
        
        glm::vec2 command_client_character_move::get_delta() const
        {
            return m_delta;
        }
        
        bool command_client_character_move::is_moving() const
        {
            return m_is_moving;
        }
        
        ui32 command_client_character_move::get_udid() const
        {
            return m_udid;
        }
    }
}
