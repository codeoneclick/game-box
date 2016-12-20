//
//  server_menu_scene.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "server_menu_scene.h"
#include "ns_ui_commands.h"
#include "ui_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera.h"
#include "button.h"
#include "console.h"
#include "game_commands_container.h"
#include "ces_server_component.h"
#include "ces_server_broadcast_component.h"
#include "ces_network_system.h"

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
        
        server_menu_scene::get_transition()->add_system(std::make_shared<gb::net::ces_network_system>());
        
        gb::net::ces_server_broadcast_component_shared_ptr server_broadcast_component = std::make_shared<gb::net::ces_server_broadcast_component>();
        server_broadcast_component->set_log_callback(std::bind(&server_menu_scene::on_log_server_message, this,
                                                               std::placeholders::_1, std::placeholders::_2));
        server_broadcast_component->start();
        server_menu_scene::add_component(server_broadcast_component);
        
        gb::net::ces_server_component_shared_ptr server_component = std::make_shared<gb::net::ces_server_component>();
        server_component->set_log_callback(std::bind(&server_menu_scene::on_log_server_message, this,
                                                     std::placeholders::_1, std::placeholders::_2));
        server_component->start();
        server_menu_scene::add_component(server_component);
        
        m_console = m_ui_fabricator->create_console(glm::vec2(server_menu_scene::get_transition()->get_screen_width(),
                                                              server_menu_scene::get_transition()->get_screen_height()), 40);
        server_menu_scene::add_child(m_console);
        
        gb::ui::button_shared_ptr back_button = m_ui_fabricator->create_button(glm::vec2(256.f, 32.f), nullptr);
        back_button->position = glm::vec2(server_menu_scene::get_transition()->get_screen_width() * .5f - 128.f, 128.f);
        back_button->set_text("back");
        server_menu_scene::add_child(back_button);
    }
    
    void server_menu_scene::on_log_server_message(const std::string& message, gb::ces_entity_const_shared_ptr entity)
    {
        m_console->write(message);
    }
}
