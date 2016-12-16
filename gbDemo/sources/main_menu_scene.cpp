//
//  main_menu_scene.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "main_menu_scene.h"
#include "ui_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera.h"
#include "button.h"
#include "game_commands_container.h"
#include "ns_ui_commands.h"

namespace ns
{
    main_menu_scene::main_menu_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
        
    }
    
    main_menu_scene::~main_menu_scene()
    {
        
    }
    
    void main_menu_scene::create()
    {
        gb::scene_graph::create();
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(main_menu_scene::get_fabricator());
        
        m_camera = std::make_shared<gb::camera>(main_menu_scene::get_transition()->get_screen_width(),
                                                main_menu_scene::get_transition()->get_screen_height());
        main_menu_scene::set_camera(m_camera);
        
        gb::ui::button_shared_ptr server_menu_button = m_ui_fabricator->create_button(glm::vec2(256.f, 32.f), std::bind(&main_menu_scene::on_goto_server_menu,
                                                                                                                        this, std::placeholders::_1));
        server_menu_button->position = glm::vec2(main_menu_scene::get_transition()->get_screen_width() * .5f - 128.f, 128.f);
        server_menu_button->set_text("create game");
        main_menu_scene::add_child(server_menu_button);
        
        gb::ui::button_shared_ptr client_menu_button = m_ui_fabricator->create_button(glm::vec2(256.f, 32.f), std::bind(&main_menu_scene::on_goto_client_menu,
                                                                                                                        this, std::placeholders::_1));
        client_menu_button->position = glm::vec2(main_menu_scene::get_transition()->get_screen_width() * .5f - 128.f, 164.f);
        client_menu_button->set_text("connect to game");
        main_menu_scene::add_child(client_menu_button);
    }
    
    void main_menu_scene::on_goto_server_menu(gb::ces_entity_const_shared_ptr entity)
    {
        if(m_external_commands)
        {
            m_external_commands->execute<on_goto_server_menu::t_command>(on_goto_server_menu::guid);
        }
        else
        {
            assert(false);
        }
    }
    
    void main_menu_scene::on_goto_client_menu(gb::ces_entity_const_shared_ptr entity)
    {
        if(m_external_commands)
        {
            m_external_commands->execute<on_goto_client_menu::t_command>(on_goto_client_menu::guid);
        }
        else
        {
            assert(false);
        }
    }
}
