//
//  demo_game_transition.cpp
//  gbDemo
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "demo_game_transition.h"
#include "demo_game_scene.h"
#include "ces_input_system.h"
#include "ces_system_types.h"
#include "koth_game_commands.h"
#include "game_commands_container.h"
#include "game_object_navigator.h"
#include "ces_koth_system_types.h"
#include "ces_camera_navigation_system.h"

#define key_w 119
#define key_a 97
#define key_d 100
#define key_s 115
#define key_1 49
#define key_2 50
#define key_3 51

demo_game_transition::demo_game_transition(const std::string& guid, bool is_offscreen) :
game_transition(guid, is_offscreen)
{
    m_keys_state[key_w] = false;
    m_keys_state[key_a] = false;
    m_keys_state[key_d] = false;
    m_keys_state[key_s] = false;
    
    m_keys_state[key_1] = false;
    m_keys_state[key_2] = false;
    m_keys_state[key_3] = false;
}

demo_game_transition::~demo_game_transition(void)
{
    
}

void demo_game_transition::create_scene()
{
    m_scene = std::make_shared<demo_game_scene>(shared_from_this());
    std::shared_ptr<gb::ces_input_system> input_system = std::static_pointer_cast<gb::ces_input_system>(game_transition::get_system(gb::ces_system_type.ces_system_type_input));
    input_system->add_touch_listener(std::static_pointer_cast<demo_game_scene>(m_scene));
    
    koth::ces_camera_navigation_system_shared_ptr camera_navigation_system = std::make_shared<koth::ces_camera_navigation_system>();
    gb::game_transition::get_input_context()->add_listener(camera_navigation_system);
    gb::game_transition::add_system(camera_navigation_system);
}

void demo_game_transition::destroy_scene()
{
    m_scene = nullptr;
}

void demo_game_transition::on_key_down(i32 key)
{
    m_keys_state[key] = true;
    demo_game_transition::update_key_state();
}

void demo_game_transition::on_key_up(i32 key)
{
    m_keys_state[key] = false;
    demo_game_transition::update_key_state();
}

void demo_game_transition::on_gr_dragged(const glm::ivec2& point, const glm::ivec2& delta, gb::e_input_element input_element)
{
    gb::game_commands_container_shared_ptr game_commands = m_scene->get_internal_commands();
    game_commands->execute<koth::on_mouse_dragged::t_command>(koth::on_mouse_dragged::guid, delta);
}

void demo_game_transition::update_key_state()
{
    gb::game_commands_container_shared_ptr game_commands = m_scene->get_internal_commands();
    if(m_keys_state[key_s] && m_keys_state[key_a])
    {
        game_commands->execute<koth::on_rotate_state_changed::t_command>(koth::on_rotate_state_changed::guid, koth::e_navigation_state_rotate_left);
        game_commands->execute<koth::on_move_state_changed::t_command>(koth::on_move_state_changed::guid, koth::e_navigation_state_move_backward);
    }
    else if(m_keys_state[key_w] && m_keys_state[key_d])
    {
        game_commands->execute<koth::on_rotate_state_changed::t_command>(koth::on_rotate_state_changed::guid, koth::e_navigation_state_rotate_right);
        game_commands->execute<koth::on_move_state_changed::t_command>(koth::on_move_state_changed::guid, koth::e_navigation_state_move_forward);
    }
    else if(m_keys_state[key_s] && m_keys_state[key_d])
    {
        game_commands->execute<koth::on_rotate_state_changed::t_command>(koth::on_rotate_state_changed::guid, koth::e_navigation_state_rotate_right);
        game_commands->execute<koth::on_move_state_changed::t_command>(koth::on_move_state_changed::guid, koth::e_navigation_state_move_backward);
    }
    else if(m_keys_state[key_w] && m_keys_state[key_a])
    {
        game_commands->execute<koth::on_rotate_state_changed::t_command>(koth::on_rotate_state_changed::guid, koth::e_navigation_state_rotate_left);
        game_commands->execute<koth::on_move_state_changed::t_command>(koth::on_move_state_changed::guid, koth::e_navigation_state_move_forward);
    }
    else if(m_keys_state[key_d])
    {
        game_commands->execute<koth::on_rotate_state_changed::t_command>(koth::on_rotate_state_changed::guid, koth::e_navigation_state_rotate_right);
        game_commands->execute<koth::on_move_state_changed::t_command>(koth::on_move_state_changed::guid, koth::e_navigation_state_move_none);
    }
    else if(m_keys_state[key_a])
    {
        game_commands->execute<koth::on_rotate_state_changed::t_command>(koth::on_rotate_state_changed::guid, koth::e_navigation_state_rotate_left);
        game_commands->execute<koth::on_move_state_changed::t_command>(koth::on_move_state_changed::guid, koth::e_navigation_state_move_none);
    }
    else if(m_keys_state[key_s])
    {
        game_commands->execute<koth::on_rotate_state_changed::t_command>(koth::on_rotate_state_changed::guid, koth::e_navigation_state_rotate_none);
        game_commands->execute<koth::on_move_state_changed::t_command>(koth::on_move_state_changed::guid, koth::e_navigation_state_move_backward);
    }
    else if(m_keys_state[key_w])
    {
        game_commands->execute<koth::on_rotate_state_changed::t_command>(koth::on_rotate_state_changed::guid, koth::e_navigation_state_rotate_none);
        game_commands->execute<koth::on_move_state_changed::t_command>(koth::on_move_state_changed::guid, koth::e_navigation_state_move_forward);
    }
    else
    {
        game_commands->execute<koth::on_rotate_state_changed::t_command>(koth::on_rotate_state_changed::guid, koth::e_navigation_state_rotate_none);
        game_commands->execute<koth::on_move_state_changed::t_command>(koth::on_move_state_changed::guid, koth::e_navigation_state_move_none);
    }
    
    if(m_keys_state[key_1])
    {
        game_commands->execute<koth::on_use_ability::t_command>(koth::on_use_ability::guid, koth::e_navigation_state_rotate_none);
    }
    if(m_keys_state[key_2])
    {
        game_commands->execute<koth::on_use_ability::t_command>(koth::on_use_ability::guid, koth::e_navigation_state_rotate_none);
    }
    if(m_keys_state[key_3])
    {
        game_commands->execute<koth::on_use_ability::t_command>(koth::on_use_ability::guid, koth::e_navigation_state_rotate_none);
    }
}
