//
//  game_conroller_osx.cpp
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "game_conroller_osx.h"
#include "main_game_controller.h"
#include "window_impl.h"

namespace game
{
    game_conroller_osx::game_conroller_osx(NSView *ogl_view)
    {
        std::shared_ptr<gb::window_impl> window = std::make_shared<gb::window_impl>((__bridge void*)ogl_view);
        m_game_controller = std::make_shared<main_game_controller>(window);
    }
    
#if USED_GRAPHICS_API == METAL_API
    
    game_conroller_osx::game_conroller_osx(MTKView* mtl_view)
    {
        std::shared_ptr<gb::window_impl> window = std::make_shared<gb::window_impl>((__bridge void*)mtl_view);
        m_game_controller = std::make_shared<main_game_controller>(window);
    }
    
#endif
    
    game_conroller_osx::~game_conroller_osx()
    {
        
    }
}
