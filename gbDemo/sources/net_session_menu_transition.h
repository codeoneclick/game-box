//
//  net_session_menu_transition.h
//  gbDemo
//
//  Created by serhii serhiiv on 4/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_transition.h"
#include "ns_declarations.h"

namespace game
{
    class net_session_menu_transition : public gb::game_transition
    {
    private:
        
    protected:
        
        void create_scene();
        void destroy_scene();
        
    public:
        
        net_session_menu_transition(const std::string& guid, bool is_offscreen);
        ~net_session_menu_transition();
    };
};
