//
//  ces_touch_system.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "ces_base_system.h"
#include "input_context.h"

namespace gb
{
    class ces_touch_system : public ces_base_system, public input_context_listener
    {
    private:
        
        typedef std::tuple<e_input_source, e_input_state, glm::ivec2, glm::ivec2, ui32> touch_event_t;
        
        std::queue<touch_event_t> m_events;
        std::array<std::set<ces_entity_weak_ptr, ces_entity_weak_comparator>, 2> m_captured_entities;
        
    protected:
        
        void on_feed_start(f32 deltatime);
        void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
        void on_feed_end(f32 deltatime);
        
        ces_entity_shared_ptr intersected_entity(const ces_entity_shared_ptr& entity,
                                                 const touch_event_t& event);
        
        void on_gr_pressed(const glm::ivec2& touch_point,
                           const glm::ivec2& touch_area_size,
                           e_input_source input_source,
                           ui32 index);
        void on_gr_released(const glm::ivec2& touch_point,
                            const glm::ivec2& touch_area_size,
                            e_input_source input_source,
                            ui32 index);
        void on_gr_moved(const glm::ivec2& touch_point,
                         const glm::ivec2& touch_area_size,
                         const glm::ivec2& delta,
                         ui32 index);
        void on_gr_dragged(const glm::ivec2& touch_point,
                           const glm::ivec2& touch_area_size,
                           const glm::ivec2& delta,
                           e_input_source input_source,
                           ui32 index);
        
        void on_key_up(i32 key);
        void on_key_down(i32 key);
        
        void on_virtual_keyboard_input(const std::string& symbol);
        void on_virtual_keyboard_backspace();
        void on_virtual_keyboard_hidden();
        
        glm::vec2 convert_touch_point_to_viewport_space(const touch_event_t& touch_event);
        
    public:
        
        CTTI_CLASS_GUID(ces_touch_system, ces_base_system::g_guids_container)
        ces_touch_system();
        ~ces_touch_system();
    };
};

#endif
