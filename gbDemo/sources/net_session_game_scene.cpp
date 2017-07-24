//
//  in_game_scene.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 12/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "net_session_game_scene.h"
#include "ui_fabricator.h"
#include "game_transition.h"
#include "scene_fabricator.h"
#include "camera_2d.h"
#include "button.h"
#include "joystick.h"
#include "game_commands_container.h"
#include "ces_box2d_body_component.h"
#include "ns_ui_commands.h"
#include "anim_fabricator.h"
#include "animated_sprite.h"
#include "light_source_2d.h"
#include "client_main_character_controller.h"
#include "client_base_character_controller.h"
#include "ces_ani_animation_system.h"
#include "ces_client_ping_component.h"
#include "ces_character_controllers_system.h"
#include "vbo.h"
#include "ces_convex_hull_component.h"
#include "ces_shadow_component.h"
#include "ces_transformation_extension.h"
#include "ces_client_component.h"
#include "ces_network_system.h"
#include "ces_battle_system.h"
#include "command_client_connection_established.h"
#include "command_client_join.h"
#include "command_character_spawn.h"
#include "command_client_character_move.h"
#include "command_server_character_move.h"
#include "command_client_character_shoot.h"
#include "command_server_character_shoot.h"

namespace game
{
	net_session_game_scene::net_session_game_scene(const gb::game_transition_shared_ptr& transition) :
    gb::scene_graph(transition)
    {
        //m_current_character_udid = -1;
    }
    
	net_session_game_scene::~net_session_game_scene()
    {
        
    }
    
    void net_session_game_scene::create()
    {
        /*gb::scene_graph::create();
        
        auto character_controllers_system = std::make_shared<ces_character_controllers_system>();
        character_controllers_system->set_order(1);
        net_session_game_scene::get_transition()->add_system(character_controllers_system);
        
        auto battle_system = std::make_shared<ces_battle_system>();
        battle_system->set_order(2);
        net_session_game_scene::get_transition()->add_system(battle_system);
        
        auto network_system = std::make_shared<gb::net::ces_network_system>();
        net_session_game_scene::get_transition()->add_system(network_system);
        
        network_system->register_command_callback(gb::net::command::k_command_client_connection_established,
                                                  std::bind(&net_session_game_scene::on_connection_established_command, this,
                                                            std::placeholders::_1));
        
        network_system->register_command_callback(gb::net::command::k_command_character_spawn,
                                                  std::bind(&net_session_game_scene::on_character_spawn_command, this,
                                                            std::placeholders::_1));
        
        network_system->register_command_callback(gb::net::command::k_command_server_character_move,
                                                  std::bind(&net_session_game_scene::on_character_move_command, this,
                                                            std::placeholders::_1));
        
        gb::net::ces_client_component_shared_ptr client_component = std::make_shared<gb::net::ces_client_component>();
        client_component->connect("178.151.163.50", 6868);
        net_session_game_scene::add_component(client_component);
        
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(net_session_game_scene::get_fabricator());
        
        m_camera_2d = std::make_shared<gb::camera_2d>(net_session_game_scene::get_transition()->get_screen_width(),
                                                      net_session_game_scene::get_transition()->get_screen_height());
        net_session_game_scene::set_camera_2d(m_camera_2d);
        
        auto animation_system = std::make_shared<gb::anim::ces_ani_animation_system>();
        animation_system->init();
        animation_system->set_order(3);
        net_session_game_scene::get_transition()->add_system(animation_system);
        
        m_anim_fabricator = std::make_shared<gb::anim::anim_fabricator>(net_session_game_scene::get_fabricator());
        
        auto level = std::make_shared<game::level>();
        level->setup("ns_level_01.xml",
                     m_camera_2d,
                     std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                     net_session_game_scene::get_fabricator(),
                     m_anim_fabricator,
                     glm::ivec2(1024),
                     glm::ivec2(64));
        net_session_game_scene::add_child(level);
        m_level = level;*/
    }
    
    void net_session_game_scene::on_connection_established_command(gb::net::command_const_shared_ptr command)
    {
        /*gb::net::command_client_connection_established_shared_ptr current_command = std::static_pointer_cast<gb::net::command_client_connection_established>(command);
        m_current_character_udid = current_command->udid;
        
        auto command_join = std::make_shared<gb::net::command_client_join>(m_current_character_udid);
        auto client_component = net_session_game_scene::get_component<gb::net::ces_client_component>();
        client_component->send_command(command_join);*/
    }
    
    void net_session_game_scene::on_character_spawn_command(gb::net::command_const_shared_ptr command)
    {
        /*gb::net::command_character_spawn_shared_ptr current_command =  std::static_pointer_cast<gb::net::command_character_spawn>(command);
        ui32 current_udid = current_command->udid;
        
        if(current_udid == m_current_character_udid)
        {
            auto move_joystick = m_ui_fabricator->create_joystick(glm::vec2(128.f));
            move_joystick->position = glm::vec2(48.f,
                                                net_session_game_scene::get_transition()->get_screen_height() - 176.f);
            move_joystick->tag = "move_joystick";
            net_session_game_scene::add_child(move_joystick);
            
            auto shoot_joystick = m_ui_fabricator->create_joystick(glm::vec2(128.f));
            shoot_joystick->position = glm::vec2(net_session_game_scene::get_transition()->get_screen_width() - 176.f,
                                                 net_session_game_scene::get_transition()->get_screen_height() - 176.f);
            shoot_joystick->tag = "shoot_joystick";
            net_session_game_scene::add_child(shoot_joystick);
            
            auto character_controller = std::make_shared<game::client_main_character_controller>(true,
                                                                                                 m_camera_2d,
                                                                                                 std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                                 net_session_game_scene::get_fabricator(),
                                                                                                 m_anim_fabricator,
                                                                                                 m_level.lock()->layers);
            //character_controller->setup("ns_character_01.xml");
            character_controller->position = current_command->position;
            character_controller->rotation = current_command->rotation;
            //character_controller->synchronize_transformations(0, current_command->position,
            //                                                  current_command->rotation);
            
            character_controller->set_map_size(glm::vec2(1024.f));

            
            //net_session_game_scene::apply_box2d_physics(character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            //    component->shape = gb::ces_box2d_body_component::circle;
            //    component->set_radius(32.f);
            //});
            net_session_game_scene::add_child(character_controller);
            
            m_main_character_controller = character_controller;*/
            /*m_main_character_controller->set_character_move_callback(std::bind(&net_session_game_scene::on_main_character_move,
                                                                               this, std::placeholders::_1,
                                                                               std::placeholders::_2,
                                                                               std::placeholders::_3));
            m_main_character_controller->set_character_shoot_callback(std::bind(&net_session_game_scene::on_main_character_shoot,
                                                                               this, std::placeholders::_1,
                                                                               std::placeholders::_2));*/
        /*}
        else
        {
            auto character_controller = std::make_shared<game::client_base_character_controller>(std::static_pointer_cast<gb::scene_graph>(shared_from_this()),
                                                                                                 net_session_game_scene::get_fabricator(),
                                                                                                 m_anim_fabricator,
                                                                                                 m_level.lock()->layers);
            //character_controller->setup("ns_character_01.xml");
            //net_session_game_scene::apply_box2d_physics(character_controller, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            //    component->shape = gb::ces_box2d_body_component::circle;
            //    component->set_radius(32.f);
            //});
            net_session_game_scene::add_child(character_controller);
            m_base_character_controllers[current_udid] = character_controller;
        }*/
    }
    
    void net_session_game_scene::on_main_character_move(ui64 move_revision, f32 move_angle, f32 dt)
    {
        /*gb::net::command_client_character_move_shared_ptr command =
        std::make_shared<gb::net::command_client_character_move>(move_revision, m_current_character_udid, move_angle, dt);
        auto client_component = net_session_game_scene::get_component<gb::net::ces_client_component>();
        client_component->send_command(command);*/
    }
    
    void net_session_game_scene::on_main_character_shoot(ui64 shoot_revision, f32 shoot_angle)
    {
        /*gb::net::command_client_character_shoot_shared_ptr command =
        std::make_shared<gb::net::command_client_character_shoot>(shoot_revision, m_current_character_udid, shoot_angle);
        auto client_component = net_session_game_scene::get_component<gb::net::ces_client_component>();
        client_component->send_command(command);*/
    }
    
    void net_session_game_scene::on_character_move_command(gb::net::command_const_shared_ptr command)
    {
        /*gb::net::command_server_character_move_shared_ptr current_command =  std::static_pointer_cast<gb::net::command_server_character_move>(command);
        ui32 current_udid = current_command->udid;
        if(current_udid != m_current_character_udid)
        {
            auto character_controller = m_base_character_controllers.find(current_udid);
            assert(character_controller != m_base_character_controllers.end());
            if(character_controller != m_base_character_controllers.end())
            {
                glm::vec2 velocity = current_command->velocity;
                glm::vec2 position = current_command->position;
                f32 rotation = current_command->rotation;
                character_controller->second->on_changed_server_transformation(velocity,
                                                                               position,
                                                                               rotation);
            }
        }
        else
        {
            ui64 move_revision = current_command->move_revision;
            glm::vec2 position = current_command->position;
            f32 rotation = current_command->rotation;*/
            /*m_main_character_controller->synchronize_transformations(move_revision,
                                                                     position,
                                                                     rotation);*/
        //}
    }
    
    void net_session_game_scene::on_character_shoot_command(gb::net::command_const_shared_ptr command)
    {
        
    }
}
