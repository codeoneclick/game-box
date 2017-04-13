//
//  net_session_menu.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 4/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "net_session_menu_scene.h"
#include "ui_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera.h"
#include "button.h"
#include "textfield.h"
#include "game_commands_container.h"
#include "ns_ui_commands.h"

namespace game
{
    net_session_menu_scene::net_session_menu_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
        
    }
    
    net_session_menu_scene::~net_session_menu_scene()
    {
        
    }
    
    void net_session_menu_scene::create()
    {
        gb::scene_graph::create();
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(net_session_menu_scene::get_fabricator());
        
        m_camera = std::make_shared<gb::camera>(net_session_menu_scene::get_transition()->get_screen_width(),
                                                net_session_menu_scene::get_transition()->get_screen_height());
        net_session_menu_scene::set_camera(m_camera);
        
        auto public_server_ip_label = m_ui_fabricator->create_textfield(glm::vec2(net_session_menu_scene::get_transition()->get_screen_width() - 16.f, 32.f), "public server: 178.151.163.50:6868");
        public_server_ip_label->position = glm::vec2(8, 8);
        public_server_ip_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_left);
        net_session_menu_scene::add_child(public_server_ip_label);
        
        auto public_server_status_label = m_ui_fabricator->create_textfield(glm::vec2(net_session_menu_scene::get_transition()->get_screen_width() - 16.f, 32.f), "status: ONLINE");
        public_server_status_label->position = glm::vec2(8, 40.f);
        public_server_status_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_left);
        net_session_menu_scene::add_child(public_server_status_label);
    }
    
    void net_session_menu_scene::on_goto_net_game_scene(gb::ces_entity_const_shared_ptr entity)
    {

    }
    
    void net_session_menu_scene::on_goto_previous_scene(gb::ces_entity_const_shared_ptr entity)
    {
    }
}
