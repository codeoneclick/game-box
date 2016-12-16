//
//  server_menu_transition.h
//  gbDemo
//
//  Created by serhii serhiiv on 12/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_transition.h"
#include "ns_declarations.h"

namespace ns
{
    class server_menu_transition : public gb::game_transition
    {
    private:
        
    protected:
        
        void create_scene();
        void destroy_scene();
        
    public:
        
        server_menu_transition(const std::string& guid, bool is_offscreen);
        ~server_menu_transition();
    };
};

