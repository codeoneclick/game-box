//
//  ns_declarations.h
//  gbDemo
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "declarations.h"

#if !defined(__NO_RENDER__)

#include "ui_declarations.h"

#endif

#include "ani_declarations.h"
#include "net_declarations.h"

namespace ns
{
    forward_decl(main_game_controller)
    forward_decl(main_menu_transition)
    forward_decl(main_menu_scene)
    forward_decl(server_menu_transition)
    forward_decl(server_menu_scene)
    forward_decl(client_menu_transition)
    forward_decl(client_menu_scene)
    forward_decl(in_game_transition)
    forward_decl(in_game_scene)
    forward_decl(client_main_character_controller)
    forward_decl(client_base_character_controller)
    forward_decl(server_character_controller)
};

