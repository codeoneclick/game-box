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
            
            ui32 m_udid;
            glm::vec2 m_delta;
            bool m_is_moving;
            
        public:
            
            command_client_character_move();
            command_client_character_move(ui32 udid, const glm::vec2& delta, bool is_moving);
            ~command_client_character_move();
            
            static command_client_character_move_shared_ptr create(std::streambuf&& buffer, i32 size);
            
            std::streambuf& serialize() override;
            void deserialize(std::streambuf&& buffer, i32 size) override;
            
            ui32 get_udid() const;
            glm::vec2 get_delta() const;
            bool is_moving() const;
        };
    };
};
