//
//  command_client_character_move.h
//  gbNetwork
//
//  Created by serhii serhiiv on 1/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "command.h"

namespace gb
{
    namespace net
    {
        class command_client_character_move : public command
        {
        private:
            
        protected:
            
            ui64 m_client_tick;
            i32 m_udid;
            glm::vec2 m_delta;
            
        public:
            
            command_client_character_move();
            command_client_character_move(ui64 client_tick, i32 udid, const glm::vec2& delta);
            ~command_client_character_move();
            
            static command_client_character_move_shared_ptr create(const std::shared_ptr<std::streambuf>& buffer);
            void init() override;
            
            std::shared_ptr<std::streambuf> serialize() override;
            void deserialize(const std::shared_ptr<std::streambuf>& buffer) override;
            
            std::property_ro<ui64> client_tick;
            std::property_ro<i32> udid;
            std::property_ro<glm::vec2> delta;
        };
    };
};
