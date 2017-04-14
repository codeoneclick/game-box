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
            m_move_revision = std::numeric_limits<ui64>::max();
            m_udid = -1;
            m_move_angle = 0.f;
            m_dt = 1.f;
        }
        
        command_client_character_move::command_client_character_move(ui64 move_revision,
                                                                     i32 udid,
                                                                     f32 move_angle,
                                                                     f32 dt)
        {
            command_client_character_move::init();
            m_move_revision = move_revision;
            m_udid = udid;
            m_move_angle = move_angle;
            m_dt = dt;
        }
        
        command_client_character_move::~command_client_character_move()
        {
            
        }
        
        command_client_character_move_shared_ptr command_client_character_move::create(const std::shared_ptr<std::streambuf>& buffer)
        {
            command_client_character_move_shared_ptr command = std::make_shared<command_client_character_move>();
            command->deserialize(buffer);
            return command;
        }
        
        void command_client_character_move::init()
        {
            command_id.getter([=] {
                return command::k_command_client_character_move;
            });
            description.getter([=] {
                std::stringstream str_stream;
                str_stream<<"move_revision: "<<m_move_revision<<"; ";
                str_stream<<"udid: "<<m_udid<<"; ";
                str_stream<<"move_angle: "<<m_move_angle<<"; ";
                str_stream<<"deltatime: "<<m_dt<<";";
                return str_stream.str();
            });
            
            move_revision.getter([=] {
                return m_move_revision;
            });
            
            udid.getter([=] {
                return m_udid;
            });
            
            move_angle.getter([=] {
                return m_move_angle;
            });
            
            dt.getter([=] {
                return m_dt;
            });
        }
        
        std::shared_ptr<std::streambuf> command_client_character_move::serialize()
        {
            std::shared_ptr<std::streambuf> buffer = command::buffer;
            std::ostream stream(buffer.get());
            i32 id = command::k_command_client_character_move;
            stream.write((const char*)&id, sizeof(id));
            i32 size = sizeof(m_move_revision);
            size += sizeof(m_udid);
            size += sizeof(m_move_angle);
            size += sizeof(m_dt);
            stream.write((const char*)&size, sizeof(size));
            stream.write((const char*)&m_move_revision, sizeof(m_move_revision));
            stream.write((const char*)&m_udid, sizeof(m_udid));
            stream.write((const char*)&m_move_angle, sizeof(m_move_angle));
            stream.write((const char*)&m_dt, sizeof(m_dt));
            return buffer;
        }
        
        void command_client_character_move::deserialize(const std::shared_ptr<std::streambuf>& buffer)
        {
            std::istream stream(buffer.get());
            stream.read((char *)&m_move_revision, sizeof(m_move_revision));
            stream.read((char *)&m_udid, sizeof(m_udid));
            stream.read((char *)&m_move_angle, sizeof(m_move_angle));
            stream.read((char *)&m_dt, sizeof(m_dt));
        }
    }
}
