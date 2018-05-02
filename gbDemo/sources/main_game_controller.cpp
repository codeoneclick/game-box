//
//  main_game_controller.cpp
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "main_game_controller.h"
#include "main_menu_transition.h"
#include "local_session_game_transition.h"
#include "net_session_game_transition.h"
#include "net_session_menu_transition.h"
#include "ui_editor_transition.h"
#include "game_commands_container.h"
#include "game_command.h"
#include "ns_ui_commands.h"
#include "thread_operation.h"

namespace game
{
    main_game_controller::main_game_controller(const std::shared_ptr<gb::ogl_window>& window) :
    gb::game_controller(window)
    {
        main_game_controller::goto_main_menu_scene();
    }
    
    main_game_controller::~main_game_controller()
    {
        
    }
    
    void main_game_controller::goto_main_menu_scene()
    {
        gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
        operation->set_execution_callback([=]() {
            
            gb::game_commands_container_shared_ptr commands = std::make_shared<gb::game_commands_container>();
            
            gb::game_command_i_shared_ptr command = std::make_shared<gb::game_command<on_goto_local_session::t_command>>(std::bind(&main_game_controller::goto_local_session, this));
            commands->add_command(on_goto_local_session::guid, command);
            
            command = std::make_shared<gb::game_command<on_goto_net_menu_scene::t_command>>(std::bind(&main_game_controller::goto_net_menu_scene, this));
            commands->add_command(on_goto_net_menu_scene::guid, command);

			command = std::make_shared<gb::game_command<on_goto_ui_editor_scene::t_command>>(std::bind(&main_game_controller::goto_ui_editor_scene, this));
			commands->add_command(on_goto_ui_editor_scene::guid, command);
            
            auto transition = std::make_shared<main_menu_transition>("transition.main_menu.xml", false);
            main_game_controller::add_transition(transition);
            main_game_controller::goto_transition("transition.main_menu.xml");
            transition->set_external_commands(commands);
        });
        operation->add_to_execution_queue();
    }
    
    void main_game_controller::goto_net_menu_scene()
    {
        gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
        operation->set_execution_callback([=]() {
            
            gb::game_commands_container_shared_ptr commands = std::make_shared<gb::game_commands_container>();
            
            gb::game_command_i_shared_ptr command = std::make_shared<gb::game_command<on_goto_net_game_scene::t_command>>(std::bind(&main_game_controller::goto_net_game_scene, this));
            commands->add_command(on_goto_net_game_scene::guid, command);
            
            command = std::make_shared<gb::game_command<on_goto_main_menu_scene::t_command>>(std::bind(&main_game_controller::goto_main_menu_scene, this));
            commands->add_command(on_goto_main_menu_scene::guid, command);
            
            auto transition = std::make_shared<net_session_menu_transition>("transition.net_menu_scene.xml", false);
            main_game_controller::add_transition(transition);
            main_game_controller::goto_transition("transition.net_menu_scene.xml");
            transition->set_external_commands(commands);
        });
        operation->add_to_execution_queue();
    }
    
    void main_game_controller::goto_net_game_scene()
    {
        gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
        operation->set_execution_callback([=]() {
            auto transition = std::make_shared<net_session_game_transition>("transition.net_session.xml", false);
            main_game_controller::add_transition(transition);
            main_game_controller::goto_transition("transition.net_session.xml");
        });
        operation->add_to_execution_queue();
    }
    
    void main_game_controller::goto_local_session()
    {
        gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
        operation->set_execution_callback([=]() {
            auto transition = std::make_shared<local_session_game_transition>("transition.local_session.xml", false);
            main_game_controller::add_transition(transition);
            main_game_controller::goto_transition("transition.local_session.xml");
        });
        operation->add_to_execution_queue();
    }

	void main_game_controller::goto_ui_editor_scene() {
		gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
		operation->set_execution_callback([=]() {
			auto transition = std::make_shared<ui_editor_transition>("transition.ui_editor_scene.xml", false);
			main_game_controller::add_transition(transition);
			main_game_controller::goto_transition("transition.ui_editor_scene.xml");
		});
		operation->add_to_execution_queue();
	}
};
