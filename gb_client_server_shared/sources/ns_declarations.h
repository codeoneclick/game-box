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

namespace game
{
    forward_decl(main_game_controller)
    forward_decl(main_menu_transition)
    forward_decl(main_menu_scene)
    forward_decl(local_session_game_transition)
    forward_decl(local_session_game_scene)
    forward_decl(net_session_game_transition)
    forward_decl(net_session_game_scene)
    forward_decl(client_main_character_controller)
    forward_decl(client_base_character_controller)
    forward_decl(server_character_controller)
	forward_decl(ces_bullet_component)
	forward_decl(character)
	forward_decl(bullet)
};

