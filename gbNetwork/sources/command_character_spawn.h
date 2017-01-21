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
            
            ui32 m_udid;
            
            f32 m_position_x;
            f32 m_position_y;
            f32 m_rotation;
            
        public:
            
            command_character_spawn();
            command_character_spawn(ui32 udid, f32 position_x, f32 position_y, f32 rotation);
            ~command_character_spawn();
            
            static command_character_spawn_shared_ptr create(std::streambuf&& buffer, i32 size);
            
            std::streambuf& serialize() override;
            void deserialize(std::streambuf&& buffer, i32 size) override;
            
            ui32 get_udid() const;
            f32 get_position_x() const;
            f32 get_position_y() const;
            f32 get_rotation() const;
        };
    };
};

