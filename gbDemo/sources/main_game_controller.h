//
//  main_game_controller.h
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "game_controller.h"
#include "ns_declarations.h"

namespace game
{
    class main_game_controller : public gb::game_controller
    {
    private:
        
    protected:
        
    public:
        
        main_game_controller(const std::shared_ptr<gb::window_impl>& window);
        ~main_game_controller();
        
        void goto_main_menu_scene();
        void goto_in_game_scene();
        void goto_net_menu_scene();
        void goto_net_game_scene();
		void goto_ui_editor_scene();
    };
};
