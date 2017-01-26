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
            
            ui64 m_client_tick;
            i32 m_udid;
            glm::vec2 m_velocity;
            glm::vec2 m_position;
            f32 m_rotation;
            
        protected:
            
        public:
            
            command_server_character_move();
            command_server_character_move(ui64 client_tick, i32 udid,
                                          const glm::vec2& velocity,
                                          const glm::vec2& position,
                                          f32 rotation);
            ~command_server_character_move();
            
            static command_server_character_move_shared_ptr create(const std::shared_ptr<std::streambuf>& buffer);
            void init() override;
            
            std::shared_ptr<std::streambuf> serialize() override;
            void deserialize(const std::shared_ptr<std::streambuf>& buffer) override;
            
            std::property_ro<ui64> client_tick;
            std::property_ro<i32> udid;
            std::property_ro<glm::vec2> velocity;
            std::property_ro<glm::vec2> position;
            std::property_ro<f32> rotation;
        };
    };
};
