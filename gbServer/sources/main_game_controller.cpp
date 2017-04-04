//
//  main_game_controller.cpp
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "main_game_controller.h"
#include "in_game_transition.h"

namespace game
{
    main_game_controller::main_game_controller() :
    gb::game_controller(nullptr)
    {
        auto transition = std::make_shared<in_game_transition>("transition.server.xml", false);
        main_game_controller::add_transition(transition);
        main_game_controller::goto_transition("transition.server.xml");
    }
    
    main_game_controller::~main_game_controller()
    {
        
    }
};
