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
            
            ui64 m_move_revision;
            i32 m_udid;
            f32 m_move_angle;
            
        public:
            
            command_client_character_move();
            command_client_character_move(ui64 move_revision, i32 udid, f32 move_angle);
            ~command_client_character_move();
            
            static command_client_character_move_shared_ptr create(const std::shared_ptr<std::streambuf>& buffer);
            void init() override;
            
            std::shared_ptr<std::streambuf> serialize() override;
            void deserialize(const std::shared_ptr<std::streambuf>& buffer) override;
            
            std::property_ro<ui64> move_revision;
            std::property_ro<i32> udid;
            std::property_ro<f32> move_angle;
        };
    };
};
