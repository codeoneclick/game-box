//
//  main_game_controller.cpp
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "main_game_controller.h"
#include "main_menu_transition.h"
#include "server_menu_transition.h"
#include "client_menu_transition.h"
#include "game_commands_container.h"
#include "game_command.h"
#include "ns_ui_commands.h"
#include "thread_operation.h"

namespace ns
{
    main_game_controller::main_game_controller(const std::shared_ptr<gb::ogl_window>& window) :
    gb::game_controller(window)
    {
        
        gb::game_commands_container_shared_ptr commands = std::make_shared<gb::game_commands_container>();
        gb::game_command_i_shared_ptr command = std::make_shared<gb::game_command<on_goto_server_menu::t_command>>(std::bind(&main_game_controller::goto_server_menu, this));
        commands->add_command(on_goto_server_menu::guid, command);
        command = std::make_shared<gb::game_command<on_goto_client_menu::t_command>>(std::bind(&main_game_controller::goto_client_menu, this));
        commands->add_command(on_goto_client_menu::guid, command);
        
        m_game_transition = std::make_shared<main_menu_transition>("transition.main_menu.xml", false);
        main_game_controller::add_transition(m_game_transition);
        main_game_controller::goto_transition("transition.main_menu.xml");
        
        m_game_transition->set_external_commands(commands);
    }
    
    main_game_controller::~main_game_controller()
    {
        
    }
    
    void main_game_controller::goto_server_menu()
    {
        gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
        operation->set_execution_callback([=]() {
            server_menu_transition_shared_ptr transition = std::make_shared<server_menu_transition>("transition.server_menu.xml", false);
            main_game_controller::add_transition(transition);
            main_game_controller::goto_transition("transition.server_menu.xml");
        });
        operation->add_to_execution_queue();
    }
    
    void main_game_controller::goto_client_menu()
    {
        gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
        operation->set_execution_callback([=]() {
            client_menu_transition_shared_ptr transition = std::make_shared<client_menu_transition>("transition.client_menu.xml", false);
            main_game_controller::add_transition(transition);
            main_game_controller::goto_transition("transition.client_menu.xml");
        });
        operation->add_to_execution_queue();
    }
};
