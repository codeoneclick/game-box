//
//  demo_game_controller.cpp
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "demo_game_controller.h"

using namespace gb;

demo_game_controller::demo_game_controller(const std::shared_ptr<gb::ogl_window>& window) :
gb::game_controller(window)
{

}

demo_game_controller::~demo_game_controller(void)
{
    
}