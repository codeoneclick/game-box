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

namespace gb
{
    forward_decl(gameplay_configuration_accessor)
};

namespace game
{
    forward_decl(main_game_controller)
    forward_decl(main_menu_transition)
    forward_decl(main_menu_scene)
    forward_decl(local_session_game_transition)
    forward_decl(local_session_game_scene)
    forward_decl(net_session_game_transition)
    forward_decl(net_session_game_scene)
	forward_decl(ces_hit_bounds_component)
    forward_decl(ces_character_battle_component)
    forward_decl(pathfinder)
    forward_decl(path_map)
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
    forward_decl(characters_3d_assembler)
    forward_decl(hit_bounds_controller)
    forward_decl(gameplay_fabricator)
    forward_decl(gameplay_ui_fabricator)
    forward_decl(ces_character_controllers_component)
    forward_decl(db_characters_table)

	forward_decl(ui_editable_fabricator)
	forward_decl(editable_button)
	forward_decl(editable_textfield)
};

