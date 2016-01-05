//
//  ui_to_in_game_scene_commands.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ui_to_in_game_scene_commands_h
#define ui_to_in_game_scene_commands_h

#include "main_headers.h"
#include "declarations.h"

namespace koth
{
    namespace keyboard_on_key_down
    {
        typedef std::function<void(i32)> t_command;
        const std::string guid = "keyboard_on_key_down";
    };
    
    namespace keyboard_on_key_up
    {
        typedef std::function<void(i32)> t_command;
        const std::string guid = "keyboard_on_key_up";
    };
    
    namespace on_move_state_changed
    {
        typedef std::function<void(i32)> t_command;
        const std::string guid = "on_move_state_changed";
    };
    
    namespace on_rotate_state_changed
    {
        typedef std::function<void(i32)> t_command;
        const std::string guid = "on_rotate_state_changed";
    };
    
    namespace on_use_ability
    {
        typedef std::function<void(i32)> t_command;
        const std::string guid = "on_use_ability";
    };
    
    namespace on_mouse_dragged
    {
        typedef std::function<void(const glm::ivec2&)> t_command;
        const std::string guid = "on_mouse_dragged";
    };
};

#endif
