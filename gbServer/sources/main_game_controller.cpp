//
//  main_game_controller.cpp
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "main_game_controller.h"
#include "in_game_transition.h"
#include "game_commands_container.h"
#include "game_command.h"
#include "ns_ui_commands.h"
#include "thread_operation.h"

namespace ns
{
    main_game_controller::main_game_controller(const std::shared_ptr<gb::ogl_window>& window) :
    gb::game_controller(window)
    {
        m_game_transition = std::make_shared<in_game_transition>("transition.in_game.xml", false);
        main_game_controller::add_transition(m_game_transition);
        main_game_controller::goto_transition("transition.in_game.xml");
    }
    
    main_game_controller::~main_game_controller()
    {
        
    }
};
