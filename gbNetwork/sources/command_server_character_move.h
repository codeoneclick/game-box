//
//  command_server_character_move.h
//  gbNetwork
//
//  Created by serhii serhiiv on 12/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "command.h"

namespace gb
{
    namespace net
    {
        class command_server_character_move : public command
        {
        private:
            
        protected:
            
            ui64 m_timestamp;
            ui32 m_udid;
            glm::vec2 m_velocity;
            glm::vec2 m_position;
            f32 m_rotation;
            bool m_is_moving;
            
        public:
            
            command_server_character_move();
            command_server_character_move(ui64 timestamp, ui32 udid, const glm::vec2& velocity, const glm::vec2& position,
                                          f32 rotation, bool is_moving);
            ~command_server_character_move();
            
            static command_server_character_move_shared_ptr create(std::streambuf&& buffer, i32 size);
            
            std::streambuf& serialize() override;
            void deserialize(std::streambuf&& buffer, i32 size) override;
            
            ui64 get_timestamp() const;
            ui32 get_udid() const;
            glm::vec2 get_velocity() const;
            glm::vec2 get_position() const;
            f32 get_rotation() const;
            bool is_moving() const;
        };
    };
};
