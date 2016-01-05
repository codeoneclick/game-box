//
//  common_navigator.h
//  gbDemo
//
//  Created by sergey.sergeev on 11/24/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef common_navigator_h
#define common_navigator_h

#include "main_headers.h"
#include "declarations.h"

namespace koth
{
    enum e_navigation_state_move
    {
        e_navigation_state_move_none = 0,
        e_navigation_state_move_forward,
        e_navigation_state_move_backward,
        e_navigation_state_move_left,
        e_navigation_state_move_right
    };
    
    enum e_navigation_state_rotate
    {
        e_navigation_state_rotate_none = 0,
        e_navigation_state_rotate_left,
        e_navigation_state_rotate_right,
        e_navigation_state_rotate_up,
        e_navigation_state_rotate_down
    };
    
    class common_navigator
    {
    private:
        
    protected:
        
        f32 m_move_forward_speed;
        f32 m_move_backward_speed;
        f32 m_move_side_speed;
        f32 m_rotate_speed;
        
        glm::vec3 m_position;
        glm::vec3 m_rotation;
        
        e_navigation_state_move m_navigation_state_move;
        e_navigation_state_rotate m_navigation_state_rotate;
        
    public:
        
        common_navigator(f32 move_forward_speed,
                         f32 move_backward_speed,
                         f32 move_side_speed,
                         f32 rotate_speed);
        ~common_navigator();
        
        virtual void update(f32 deltatime);
        
        virtual void set_position(const glm::vec3& position);
        virtual void set_rotation(const glm::vec3& rotation);
        
        void move_forward();
        void move_backward();
        
        void move_left();
        void move_right();
        
        void rotate_left();
        void rotate_right();
        
        void rotate_up();
        void rotate_down();
    };
};

#endif
