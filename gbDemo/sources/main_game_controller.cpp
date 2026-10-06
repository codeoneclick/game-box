//
//  main_game_controller.cpp
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "main_game_controller.h"
#include "island_demo_transition.h"
#include "thread_operation.h"

namespace game
{
    main_game_controller::main_game_controller(const std::shared_ptr<gb::window_impl>& window) :
    gb::game_controller(window)
    {
        main_game_controller::goto_island_demo_scene();
    }
    
    main_game_controller::~main_game_controller()
    {
        
    }
    
    void main_game_controller::goto_island_demo_scene()
    {
        gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
        operation->set_execution_callback([=]() {
            
            auto transition = std::make_shared<island_demo_transition>("transition.island_demo.xml", get_window(), false);
            main_game_controller::add_transition(transition);
            main_game_controller::goto_transition("transition.island_demo.xml");
        });
        operation->add_to_execution_queue();
    }
    
	void main_game_controller::goto_ui_editor_scene()
	{
		
	}
};
