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
#include "ces_client_component.h"
#include "ces_network_system.h"
#include "ns_ui_commands.h"
#include "command_client_connection_established.h"
#include "command_client_join.h"

namespace game
{
    net_session_menu_scene::net_session_menu_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition),
    m_session_udid(-1)
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
        
        auto public_server_ip_label = m_ui_fabricator->create_textfield(glm::vec2(net_session_menu_scene::get_transition()->get_screen_width() - 16.f, 32.f), " public server: 178.151.163.50:6868");
        public_server_ip_label->position = glm::vec2(8, 8);
        public_server_ip_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_left);
        net_session_menu_scene::add_child(public_server_ip_label);
        
        auto public_server_status_label = m_ui_fabricator->create_textfield(glm::vec2(116.f, 32.f), " status:");
        public_server_status_label->position = glm::vec2(8, 40.f);
        public_server_status_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_left);
        net_session_menu_scene::add_child(public_server_status_label);
        
        public_server_status_label = m_ui_fabricator->create_textfield(glm::vec2(net_session_menu_scene::get_transition()->get_screen_width() - 16.f - 116.f, 32.f), "OFFLINE");
        public_server_status_label->position = glm::vec2(8.f + 116.f, 40.f);
        public_server_status_label->set_font_color(glm::u8vec4(255, 0, 0, 255));
        public_server_status_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_left);
        net_session_menu_scene::add_child(public_server_status_label);
        m_public_server_status_label = public_server_status_label;
        
        gb::ui::button_shared_ptr goto_net_game_scene_button = m_ui_fabricator->create_button(glm::vec2(128.f, 32.f), std::bind(&net_session_menu_scene::on_goto_net_game_scene,
                                                                                                                                this, std::placeholders::_1));
        goto_net_game_scene_button->position = glm::vec2(net_session_menu_scene::get_transition()->get_screen_width() - 128.f - 8.f,
                                                         net_session_menu_scene::get_transition()->get_screen_height() - 32.f - 8.f);
        goto_net_game_scene_button->set_text("join");
        net_session_menu_scene::add_child(goto_net_game_scene_button);
        
        gb::ui::button_shared_ptr goto_main_menu_scene_button = m_ui_fabricator->create_button(glm::vec2(128.f, 32.f), std::bind(&net_session_menu_scene::on_goto_main_menu_scene,
                                                                                                                        this, std::placeholders::_1));
        goto_main_menu_scene_button->position = glm::vec2(8.f, net_session_menu_scene::get_transition()->get_screen_height() - 32.f - 8.f);
        goto_main_menu_scene_button->set_text("back");
        net_session_menu_scene::add_child(goto_main_menu_scene_button);
        
        auto network_system = std::make_shared<gb::net::ces_network_system>();
        net_session_menu_scene::get_transition()->add_system(network_system);
        
        network_system->register_command_callback(gb::net::command::k_command_client_connection_established,
                                                  std::bind(&net_session_menu_scene::on_connection_established_command, this,
                                                            std::placeholders::_1));
        
        gb::net::ces_client_component_shared_ptr client_component = std::make_shared<gb::net::ces_client_component>();
        client_component->connect("178.151.163.50", 6868);
        net_session_menu_scene::add_component(client_component);
    }
    
    void net_session_menu_scene::on_connection_established_command(gb::net::command_const_shared_ptr command)
    {
        gb::net::command_client_connection_established_shared_ptr current_command = std::static_pointer_cast<gb::net::command_client_connection_established>(command);
        m_session_udid = current_command->udid;
        if(!m_public_server_status_label.expired())
        {
            m_public_server_status_label.lock()->set_text("ONLINE");
            m_public_server_status_label.lock()->set_font_color(glm::u8vec4(0, 255, 0, 255));
        }
    }
    
    void net_session_menu_scene::on_goto_net_game_scene(gb::ces_entity_const_shared_ptr entity)
    {
        if(m_external_commands)
        {
            m_external_commands->execute<on_goto_net_game_scene::t_command>(on_goto_net_game_scene::guid);
        }
        else
        {
            assert(false);
        }
    }
    
    void net_session_menu_scene::on_goto_main_menu_scene(gb::ces_entity_const_shared_ptr entity)
    {
        if(m_external_commands)
        {
            m_external_commands->execute<on_goto_main_menu_scene::t_command>(on_goto_main_menu_scene::guid);
        }
        else
        {
            assert(false);
        }
    }
}
