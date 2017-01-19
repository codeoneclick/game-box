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
            m_udid = std::numeric_limits<ui32>::max();
            m_position_x = 0.f;
            m_position_y = 0.f;
            m_rotation = 0.f;
        }
        
        command_character_spawn::command_character_spawn(ui32 udid, f32 position_x, f32 position_y, f32 rotation)
        {
            m_udid = udid;
            m_position_x = position_x;
            m_position_y = position_y;
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
        
        std::streambuf& command_character_spawn::serialize()
        {
            std::ostream stream(&command::get_buffer());
            i32 id = command_character_spawn::class_guid();
            stream.write((const char*)&id, sizeof(id));
            
            i32 size = sizeof(m_udid);
            size += sizeof(m_position_x);
            size += sizeof(m_position_y);
            size += sizeof(m_rotation);
            
            stream.write((const char*)&size, sizeof(size));
            stream.write((const char*)&m_udid, sizeof(m_udid));
            stream.write((const char*)&m_position_x, sizeof(m_position_x));
            stream.write((const char*)&m_position_y, sizeof(m_position_y));
            stream.write((const char*)&m_rotation, sizeof(m_rotation));
            
            return command::get_buffer();
        }
        
        void command_character_spawn::deserialize(std::streambuf&& buffer, i32 size)
        {
            std::istream stream(&buffer);
            stream.read((char *)&m_udid, sizeof(m_udid));
            stream.read((char *)&m_position_x, sizeof(m_position_x));
            stream.read((char *)&m_position_y, sizeof(m_position_y));
            stream.read((char *)&m_rotation, sizeof(m_rotation));
        }
        
        ui32 command_character_spawn::get_udid() const
        {
            return m_udid;
        }
        
        f32 command_character_spawn::get_position_x() const
        {
            return m_position_x;
        }
        
        f32 command_character_spawn::get_position_y() const
        {
            return m_position_y;
        }
        
        f32 command_character_spawn::get_rotation() const
        {
            return m_rotation;
        }
    }
}
