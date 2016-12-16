//
//  server_menu_scene.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "server_menu_scene.h"
#include "ui_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera.h"
#include "button.h"
#include "console.h"
#include "game_commands_container.h"
#include "ns_ui_commands.h"

namespace ns
{
    server_menu_scene::server_menu_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
        
    }
    
    server_menu_scene::~server_menu_scene()
    {
        
    }
    
    void server_menu_scene::create()
    {
        gb::scene_graph::create();
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(server_menu_scene::get_fabricator());
        
        m_camera = std::make_shared<gb::camera>(server_menu_scene::get_transition()->get_screen_width(),
                                                server_menu_scene::get_transition()->get_screen_height());
        server_menu_scene::set_camera(m_camera);
        
        gb::ui::console_shared_ptr console = m_ui_fabricator->create_console(glm::vec2(server_menu_scene::get_transition()->get_screen_width(),
                                                                                       server_menu_scene::get_transition()->get_screen_height()), 20);
        server_menu_scene::add_child(console);
        
        gb::ui::button_shared_ptr back_button = m_ui_fabricator->create_button(glm::vec2(256.f, 32.f), nullptr);
        back_button->position = glm::vec2(server_menu_scene::get_transition()->get_screen_width() * .5f - 128.f, 128.f);
        back_button->set_text("back");
        server_menu_scene::add_child(back_button);
        
        console->write("server started:");
        console->write("at 127.0.0.1");
        console->write("waiting for clients...");
    }
}
