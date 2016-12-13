//
//  command_character_move.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "command_character_move.h"

namespace gb
{
    namespace net
    {
        command_character_move::command_character_move()
        {
            m_id = 1;
            m_angle = 0.f;
        }
        
        command_character_move::command_character_move(f32 angle)
        {
            m_id = 1;
            m_angle = angle;
        }
        
        command_character_move::~command_character_move()
        {
            
        }
        
        command_character_move_shared_ptr command_character_move::create(asio::streambuf&& buffer)
        {
            command_character_move_shared_ptr command = std::make_shared<command_character_move>();
            command->deserialize(std::move(buffer));
            return command;
        }
        
        asio::streambuf& command_character_move::serialize()
        {
            std::ostream stream(&m_buffer);
            stream.write((const char*)&m_id, sizeof(m_id));
            i32 size = sizeof(m_angle);
            stream.write((const char*)&size, sizeof(size));
            stream.write((const char*)&m_angle, sizeof(m_angle));
            return m_buffer;
        }
        
        void command_character_move::deserialize(asio::streambuf&& buffer)
        {
            std::istream stream(&buffer);
            stream.read((char *)&m_angle, sizeof(m_angle));
        }
        
        f32 command_character_move::get_angle() const
        {
            return m_angle;
        }
    }
}
