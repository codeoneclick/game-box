//
//  ces_input_system.h
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_input_system_h
#define ces_input_system_h

#include "ces_system.h"
#include "input_context.h"

namespace gb
{
    class touch_listener
    {
    private:
        
    protected:
        
    public:
        
        touch_listener(void) = default;
        virtual ~touch_listener(void) = default;
        
        virtual void on_touch(const glm::vec3& point, const glm::vec3& delta, const ces_entity_shared_ptr& listener,
                              e_input_element input_element, e_input_state input_state) = 0;
    };
    
    class ces_input_system : public ces_system, public input_context_listener
    {
    private:
        
        std::set<touch_listener_shared_ptr> m_listeners;
        std::queue<std::tuple<e_input_element, e_input_state, glm::ivec2>> m_events;
        std::list<ces_entity_shared_ptr> m_entities;
        
        bool is_entity_intersected(const ces_entity_shared_ptr& entity, const glm::ivec2& point);
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        void on_gr_pressed(const glm::ivec2& point, e_input_element input_element);
        void on_gr_released(const glm::ivec2& point, e_input_element input_element);
        void on_gr_moved(const glm::ivec2& point, const glm::ivec2& delta);
        void on_gr_dragged(const glm::ivec2& point, const glm::ivec2& delta, e_input_element input_element);
        
        void on_key_up(i32 key);
        void on_key_down(i32 key);
        
    public:
        
        ces_input_system();
        ~ces_input_system() = default;
        
        void add_touch_listener(const touch_listener_shared_ptr& listener);
        void remove_touch_listener(const touch_listener_shared_ptr& listener);
    };
};

#endif
