//
//  game_object_navigator.h
//  gbDemo
//
//  Created by sergey.sergeev on 10/7/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef game_object_navigator_h
#define game_object_navigator_h

#include "main_headers.h"
#include "declarations.h"

namespace koth
{
    enum e_navigation_state_move
    {
        e_navigation_state_move_none = 0,
        e_navigation_state_move_forward,
        e_navigation_state_move_backward
    };
    
    enum e_navigation_state_rotate
    {
        e_navigation_state_rotate_none = 0,
        e_navigation_state_rotate_left,
        e_navigation_state_rotate_right
    };
    
    class game_object_navigator
    {
    private:
        
    protected:
        
        f32 m_move_forward_speed;
        f32 m_move_backward_speed;
        f32 m_rotate_speed;
        
        glm::vec3 m_position;
        glm::vec3 m_rotation;
        
        e_navigation_state_move m_navigation_state_move;
        e_navigation_state_rotate m_navigation_state_rotate;
        
        gb::game_object_shared_ptr m_game_object;
        
    public:
        
        game_object_navigator(f32 move_forward_speed,
                              f32 move_backward_speed,
                              f32 rotate_speed,
                              const gb::game_object_shared_ptr& game_object);
        ~game_object_navigator();
        
        void update(f32 deltatime);
        
        void set_position(const glm::vec3& position);
        void set_rotation(const glm::vec3& rotation);
        
        void move_forward();
        void move_backward();
        
        void rotate_left();
        void rotate_right();
    };
};

#endif
