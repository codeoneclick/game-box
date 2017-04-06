//
//  command_server_character_shoot.h
//  gbNetwork
//
//  Created by serhii serhiiv on 4/6/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "command.h"

namespace gb
{
    namespace net
    {
        class command_server_character_shoot : public command
        {
        private:
            
            i32 m_udid;
            ui64 m_shoot_revision;
            glm::vec2 m_velocity;
            glm::vec2 m_position;
            f32 m_rotation;
            
        protected:
            
        public:
            
            command_server_character_shoot();
            command_server_character_shoot(ui64 shoot_revision, i32 udid,
                                           const glm::vec2& velocity,
                                           const glm::vec2& position,
                                           f32 rotation);
            ~command_server_character_shoot();
            
            static command_server_character_shoot_shared_ptr create(const std::shared_ptr<std::streambuf>& buffer);
            void init() override;
            
            std::shared_ptr<std::streambuf> serialize() override;
            void deserialize(const std::shared_ptr<std::streambuf>& buffer) override;
            
            std::property_ro<i32> udid;
            std::property_ro<ui64> shoot_revision;
            std::property_ro<glm::vec2> velocity;
            std::property_ro<glm::vec2> position;
            std::property_ro<f32> rotation;
        };
    };
};

