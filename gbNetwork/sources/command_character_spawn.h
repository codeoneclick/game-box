//
//  command_character_spawn.h
//  gbNetwork
//
//  Created by serhii serhiiv on 1/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "command.h"

namespace gb
{
    namespace net
    {
        class command_character_spawn : public command
        {
        private:
            
        protected:
            
            i32 m_udid;
            glm::vec2 m_position;
            f32 m_rotation;
            
        public:
            
            command_character_spawn();
            command_character_spawn(i32 udid, const glm::vec2& position, f32 rotation);
            ~command_character_spawn();
            
            static command_character_spawn_shared_ptr create(const std::shared_ptr<std::streambuf>& buffer);
            void init() override;
            
            std::shared_ptr<std::streambuf> serialize() override;
            void deserialize(const std::shared_ptr<std::streambuf>& buffer) override;
            
            std::property_ro<i32> udid;
            std::property_ro<glm::vec2> position;
            std::property_ro<f32> rotation;
        };
    };
};

