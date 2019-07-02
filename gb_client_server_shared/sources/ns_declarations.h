//
//  ns_declarations.h
//  gbDemo
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "declarations.h"
#include "main_headers.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "ui_declarations.h"

#endif

#include "net_declarations.h"

namespace gb
{
    forward_decl(gameplay_configuration_accessor)
};

namespace game
{
	forward_decl(main_game_controller);
	forward_decl(main_menu_transition);
	forward_decl(main_menu_scene);
	forward_decl(local_session_game_transition);
	forward_decl(local_session_game_scene);
	forward_decl(net_session_game_transition);
	forward_decl(net_session_game_scene);
	forward_decl(gameplay_fabricator);
	forward_decl(gameplay_ui_fabricator);
	forward_decl(ui_editable_fabricator);
	forward_decl(editable_button);
	forward_decl(editable_textfield);
};

