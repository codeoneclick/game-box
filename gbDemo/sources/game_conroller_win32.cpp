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

namespace game
{
    game_conroller_win32::game_conroller_win32()
    {
		glm::ivec2 resolution = glm::ivec2(1280, 720);
        std::shared_ptr<gb::ogl_window> window = std::make_shared<gb::ogl_window>((void*)&resolution);
        m_game_controller = std::make_shared<main_game_controller>(window);
    }
    
	game_conroller_win32::~game_conroller_win32()
    {
        
    }
}
