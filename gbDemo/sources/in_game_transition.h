//
//  in_game_transition.h
//  gbDemo
//
//  Created by serhii.s on 4/16/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_transition.h"
#include "ns_declarations.h"

namespace game
{
    class in_game_transition : public gb::game_transition
    {
    private:
        
    protected:
        
        void create_scene();
        void destroy_scene();
        
    public:
        
        in_game_transition(const std::string& guid, bool is_offscreen);
        ~in_game_transition() = default;
    };
};

