//
//  game_conroller_osx.cpp
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "game_conroller_win32.h"
#include "main_game_controller.h"
#include "ogl_window.h"

namespace ns
{
    game_conroller_win32::game_conroller_win32()
    {
        m_game_controller = std::make_shared<main_game_controller>(nullptr);
    }
    
	game_conroller_win32::~game_conroller_win32()
    {
        
    }
}
