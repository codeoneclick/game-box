//
//  game_conroller_osx.h
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"

namespace ns
{
    class game_conroller_win32
    {
    private:
        
    protected:
        
        main_game_controller_shared_ptr m_game_controller;
        main_menu_transition_shared_ptr m_game_transition;
        
    public:
        
		game_conroller_win32();
        ~game_conroller_win32();
        
    };
};
