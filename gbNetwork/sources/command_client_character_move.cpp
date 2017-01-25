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
            command_client_character_move::init();
            m_client_tick = std::numeric_limits<ui64>::max();
            m_udid = -1;
            m_delta = glm::vec2(0.f);
        }
        
        command_client_character_move::command_client_character_move(ui64 client_tick,
                                                                     i32 udid,
                                                                     const glm::vec2& delta)
        {
            command_client_character_move::init();
            m_client_tick = client_tick;
            m_udid = udid;
            m_delta = delta;
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
        
        void command_client_character_move::init()
        {
            command_id.getter([=] {
                return command::k_command_client_character_move;
            });
            description.getter([=] {
                std::stringstream str_stream;
                str_stream<<"client_tick: "<<m_client_tick<<"; ";
                str_stream<<"udid: "<<m_udid<<"; ";
                str_stream<<"delta_x: "<<m_delta.x<<"; ";
                str_stream<<"delta_y: "<<m_delta.y<<"; ";
                return str_stream.str();
            });
            
            client_tick.getter([=] {
                return m_client_tick;
            });
            
            udid.getter([=] {
                return m_udid;
            });
            
            delta.getter([=] {
                return m_delta;
            });
        }
        
        std::streambuf& command_client_character_move::serialize()
        {
            std::ostream stream(&command::get_buffer());
            i32 id = command::k_command_client_character_move;
            stream.write((const char*)&id, sizeof(id));
            i32 size = sizeof(m_client_tick);
            size += sizeof(m_udid);
            size += sizeof(m_delta);
            stream.write((const char*)&size, sizeof(size));
            stream.write((const char*)&m_client_tick, sizeof(m_client_tick));
            stream.write((const char*)&m_udid, sizeof(m_udid));
            stream.write((const char*)&m_delta, sizeof(m_delta));
            return command::get_buffer();
        }
        
        void command_client_character_move::deserialize(std::streambuf&& buffer, i32 size)
        {
            std::istream stream(&buffer);
            stream.read((char *)&m_client_tick, sizeof(m_client_tick));
            stream.read((char *)&m_udid, sizeof(m_udid));
            stream.read((char *)&m_delta, sizeof(m_delta));
        }
    }
}
