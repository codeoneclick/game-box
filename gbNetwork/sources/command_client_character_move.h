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
            
            glm::vec2 m_delta;
            bool m_is_moving;
            
        public:
            
            CTTI_CLASS_GUID(command_client_character_move, command::g_guids_container)
            
            command_client_character_move();
            command_client_character_move(const glm::vec2& delta, bool is_moving);
            ~command_client_character_move();
            
            static command_client_character_move_shared_ptr create(std::streambuf&& buffer, i32 size);
            
            std::streambuf& serialize() override;
            void deserialize(std::streambuf&& buffer, i32 size) override;
            
            glm::vec2 get_delta() const;
            bool is_moving() const;
        };
    };
};
