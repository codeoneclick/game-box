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
    forward_decl(ces_character_battle_component)
    forward_decl(ces_ai_component)
    forward_decl(ai_character_controller)
	forward_decl(character)
	forward_decl(bullet)
    forward_decl(pathfinder)
    forward_decl(path_map)
    forward_decl(level)
    forward_decl(ai_action)
    forward_decl(ai_actions_processor)
    forward_decl(ai_move_action)
    forward_decl(ai_attack_action)
    forward_decl(ai_attack_move_action)
    forward_decl(ai_chase_action)
    forward_decl(information_bubble)
    forward_decl(information_bubble_controller)
    forward_decl(bloodprint)
    forward_decl(bloodprint_controller)
    forward_decl(footprint)
    forward_decl(footprint_controller)
    forward_decl(characters_3d_controller)
    forward_decl(ability_button)
    forward_decl(attack_button)
    forward_decl(avatar_icon)
};

