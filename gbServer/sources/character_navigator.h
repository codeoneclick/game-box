//
//  character_navigator.h
//  gbDemo
//
//  Created by serhii serhiiv on 11/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"

namespace ns
{
    class character_navigator
    {
    public:
        
        enum e_move_state
        {
            e_move_state_none = 0,
            e_move_state_forward,
            e_move_state_backward,
            e_move_state_left,
            e_move_state_right
        };
        
        enum e_steer_state
        {
            e_steer_state_none = 0,
            e_steer_state_left,
            e_steer_state_right
        };
        
    private:
        
    protected:
        
        f32 m_move_forward_speed;
        f32 m_move_backward_speed;
        f32 m_strafe_speed;
        f32 m_steer_speed;
        
        e_move_state m_move_state;
        e_steer_state m_steer_state;
        
    public:
        
        character_navigator(f32 move_forward_speed, f32 move_backward_speed, f32 strafe_speed, f32 steer_speed);
        
        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        
        void set_position(const glm::vec2& position);
        void set_rotation(f32 rotation);
        
        void move_forward();
        void move_backward();
        void move_left();
        void move_right();
        
        void steer_left();
        void steer_right();
    };
};

