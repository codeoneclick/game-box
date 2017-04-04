//
//  game_conroller_osx.cpp
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "game_conroller_osx.h"
#include "main_game_controller.h"
#include "ogl_window.h"

namespace game
{
    game_conroller_osx::game_conroller_osx()
    {
        m_game_controller = std::make_shared<main_game_controller>();
    }
    
    game_conroller_osx::~game_conroller_osx()
    {
        
    }
}
