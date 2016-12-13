//
//  command_character_move.h
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
        class command_character_move : public command
        {
        private:
            
        protected:
            
            f32 m_angle;
            
        public:
            
            command_character_move();
            command_character_move(f32 angle);
            ~command_character_move();
            
            static command_character_move_shared_ptr create(asio::streambuf&& buffer);
            
            asio::streambuf& serialize() override;
            void deserialize(asio::streambuf&& buffer) override;
            
            f32 get_angle() const;
        };
    };
};
