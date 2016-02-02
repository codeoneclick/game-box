//
//  ces_touch_system.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_touch_system_h
#define ces_touch_system_h

#include "ces_system.h"
#include "input_context.h"

namespace gb
{
    class ces_touch_system : public ces_system, public input_context_listener
    {
    private:
        
        std::queue<std::tuple<e_input_source, e_input_state, glm::ivec2>> m_events;
        std::set<ces_entity_shared_ptr> m_captured_entities;
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        ces_entity_shared_ptr intersected_entity(const ces_entity_shared_ptr& entity, const std::tuple<e_input_source, e_input_state, glm::ivec2>& event);
        
        void on_gr_pressed(const glm::ivec2& point, e_input_source input_source);
        void on_gr_released(const glm::ivec2& point, e_input_source input_source);
        void on_gr_moved(const glm::ivec2& point, const glm::ivec2& delta);
        void on_gr_dragged(const glm::ivec2& point, const glm::ivec2& delta, e_input_source input_source);
        
        void on_key_up(i32 key);
        void on_key_down(i32 key);
        
    public:
        
        ces_touch_system();
        ~ces_touch_system();
    };
};

#endif
