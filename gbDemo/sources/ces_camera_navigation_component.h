//
//  ces_camera_navigation_component.h
//  gbDemo
//
//  Created by sergey.sergeev on 12/18/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_camera_navigation_component_h
#define ces_camera_navigation_component_h

#include "ces_base_component.h"

namespace koth
{
    enum e_move_navigation_state
    {
        e_move_navigation_state_none = 0,
        e_move_navigation_state_forward,
        e_move_navigation_state_backward,
        e_move_navigation_state_left,
        e_move_navigation_state_right
    };
    
    class ces_camera_navigation_component : public gb::ces_base_component
    {
    private:
        
    protected:
        
        e_move_navigation_state m_move_navigation_state;
        glm::ivec2 m_touch_point_delta;
        
        glm::vec3 m_position;
        glm::vec3 m_look_at;
        glm::vec3 m_up;
        
    public:
        
        ces_camera_navigation_component();
        virtual ~ces_camera_navigation_component();
        
        void set_move_navigation_state(e_move_navigation_state state);
        void set_touch_point_delta(const glm::ivec2& delta);
        
        void set_position(const glm::vec3& position);
        glm::vec3 get_position() const;
        
        glm::vec3 get_look_at() const;
        glm::vec3 get_up() const;
        
        virtual void update(f32 deltatime);
    };
};

#endif
