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
#include "camera_2d.h"
#include "button.h"
#include "heightmap.h"
#include "game_commands_container.h"
#include "ns_ui_commands.h"
#include "ces_sound_system.h"
#include "ces_sound_component.h"

namespace game
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

		glm::vec2 scene_2d_size = glm::vec2(667, 375);
        
        auto sound_system = std::make_shared<gb::al::ces_sound_system>();
        main_menu_scene::get_transition()->add_system(sound_system);
        
        m_ui_fabricator = std::make_shared<gb::ui::ui_fabricator>(main_menu_scene::get_fabricator());
        
        m_camera_2d = std::make_shared<gb::camera_2d>(scene_2d_size.x,
			scene_2d_size.y);
        main_menu_scene::set_camera_2d(m_camera_2d);
        
        gb::ui::button_shared_ptr local_session_button = m_ui_fabricator->create_button(glm::vec2(256.f, 32.f), std::bind(&main_menu_scene::on_goto_local_session,
                                                                                                                          this, std::placeholders::_1));
        local_session_button->position = glm::vec2(scene_2d_size.x * .5f - 128.f, 128.f);
        local_session_button->set_text("-Start Game-");
        local_session_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        main_menu_scene::add_child(local_session_button);
        
        /*gb::ui::button_shared_ptr net_session_button = m_ui_fabricator->create_button(glm::vec2(256.f, 32.f), std::bind(&main_menu_scene::on_goto_net_session,
                                                                                                                        this, std::placeholders::_1));
        net_session_button->position = glm::vec2(scene_2d_size.x * .5f - 128.f, 164.f);
        net_session_button->set_text("net session");
        net_session_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        main_menu_scene::add_child(net_session_button);
        
        
        gb::ui::button_shared_ptr goto_net_menu_scene_button = m_ui_fabricator->create_button(glm::vec2(256.f, 32.f), std::bind(&main_menu_scene::on_goto_net_menu_scene,
                                                                                                                        this, std::placeholders::_1));
        goto_net_menu_scene_button->position = glm::vec2(scene_2d_size.x * .5f - 128.f, 200.f);
        goto_net_menu_scene_button->set_text("find server");
        goto_net_menu_scene_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        main_menu_scene::add_child(goto_net_menu_scene_button);*/
        
        auto sound_component = std::make_shared<gb::al::ces_sound_component>();
        sound_component->add_sound("music_01.mp3", true);
        sound_component->trigger_sound("music_01.mp3");
        ces_entity::add_component(sound_component);
        
        auto heightmap = main_menu_scene::get_fabricator()->create_heightmap("heightmap.village.xml");
        main_menu_scene::add_child(heightmap);
    }
    
    void main_menu_scene::on_goto_local_session(gb::ces_entity_const_shared_ptr entity)
    {
        if(m_external_commands)
        {
            m_external_commands->execute<on_goto_local_session::t_command>(on_goto_local_session::guid);
        }
        else
        {
            assert(false);
        }
    }
    
    void main_menu_scene::on_goto_net_session(gb::ces_entity_const_shared_ptr entity)
    {
        if(m_external_commands)
        {
            m_external_commands->execute<on_goto_net_session::t_command>(on_goto_net_session::guid);
        }
        else
        {
            assert(false);
        }
    }
    
    void main_menu_scene::on_goto_net_menu_scene(gb::ces_entity_const_shared_ptr entity)
    {
        if(m_external_commands)
        {
            m_external_commands->execute<on_goto_net_menu_scene::t_command>(on_goto_net_menu_scene::guid);
        }
        else
        {
            assert(false);
        }
    }
}
