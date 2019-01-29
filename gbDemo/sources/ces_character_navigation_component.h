//
//  ces_character_navigation_component.hpp
//  gbDemo
//
//  Created by serhii.s on 1/23/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    enum e_character_navigation_move_state
    {
        e_character_navigation_move_state_none = 0,
        e_character_navigation_move_state_forward,
        e_character_navigation_move_state_backward,
        e_character_navigation_move_state_left,
        e_character_navigation_move_state_right
    };
    
    enum e_character_navigation_steer_state
    {
        e_character_navigation_steer_state_none = 0,
        e_character_navigation_steer_state_left,
        e_character_navigation_steer_state_right
    };
    
    class ces_character_navigation_component : public gb::ces_base_component
    {
    private:
        
    protected:
        
        e_character_navigation_move_state m_character_navigation_move_state = e_character_navigation_move_state::e_character_navigation_move_state_none;
        e_character_navigation_steer_state m_character_navigation_steer_state = e_character_navigation_steer_state::e_character_navigation_steer_state_none;
        glm::vec2 m_velocity = glm::vec2(0.f);
        f32 m_rotation = 0.f;
        
        f32 m_move_forward_speed = .1f;
        f32 m_move_backward_speed = .1f;
        f32 m_steer_speed = 1.f;
        
    public:
        
        CTTI_CLASS_GUID(ces_character_navigation_component, gb::ces_base_component::g_guids_container)
        ces_character_navigation_component();
        ~ces_character_navigation_component() = default;
        
        void set_rotation(f32 rotation);
        f32 get_rotation() const;
        
        void move_forward();
        void move_backward();
        void move_left();
        void move_right();
        
        void steer_left();
        void steer_right();
        
        void update(f32 dt);
        
        void stop_move();
        void stop_steer();
        
        glm::vec2 get_velocity() const;
        
        bool is_move() const;
    };
};

