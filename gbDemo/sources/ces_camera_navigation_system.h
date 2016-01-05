//
//  ces_camera_navigation_system.h
//  gbDemo
//
//  Created by sergey.sergeev on 12/18/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_camera_navigation_system_h
#define ces_camera_navigation_system_h

#include "ces_system.h"
#include "input_context.h"
#include "koth_declarations.h"

namespace koth
{
    class ces_camera_navigation_system : public gb::ces_system, public gb::input_context_listener
    {
    private:
        
        std::map<i32, bool> m_keys_state;
        glm::ivec2 m_touch_point_delta;
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        void on_gr_pressed(const glm::ivec2& point, gb::e_input_element input_element) { };
        void on_gr_released(const glm::ivec2& point, gb::e_input_element input_element) { };
        void on_gr_moved(const glm::ivec2& point, const glm::ivec2& delta) { };
        void on_gr_dragged(const glm::ivec2& point, const glm::ivec2& delta, gb::e_input_element input_element);
        
        void on_key_up(i32 key);
        void on_key_down(i32 key);
        
    public:
        
        ces_camera_navigation_system();
        ~ces_camera_navigation_system();
    };
};

#endif
