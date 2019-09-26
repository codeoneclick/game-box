//
//  main_menu_transition.h
//  gbDemo
//
//  Created by serhii serhiiv on 12/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_transition.h"
#include "ns_declarations.h"

namespace game
{
    class main_menu_transition : public gb::game_transition
    {
    private:
        
    protected:
        
        void create_scene();
        void destroy_scene();
        
    public:
        
        main_menu_transition(const std::string& guid,  const gb::window_impl_shared_ptr& window, bool is_offscreen);
        ~main_menu_transition();
    };
};
