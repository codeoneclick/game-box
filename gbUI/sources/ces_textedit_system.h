//
//  ces_textedit_system.hpp
//  gbUI
//
//  Created by serhii serhiiv on 4/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "ces_base_system.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class ces_textedit_system : public ces_base_system, public input_context_listener
        {
        private:
            
            typedef std::tuple<e_input_source, e_input_state, glm::ivec2, ui32> touch_event_t;
            std::queue<touch_event_t> m_events;
            ces_entity_weak_ptr m_focused_entity;
            
        protected:
            
            void on_feed_start(f32 deltatime);
            void on_feed(const ces_entity_shared_ptr& entity, f32 deltatime);
            void on_feed_end(f32 deltatime);
            
            ces_entity_shared_ptr intersected_entity(const ces_entity_shared_ptr& entity,
                                                     const touch_event_t& event);
            
            void on_gr_pressed(const glm::ivec2& point, e_input_source input_source, ui32 index);
            void on_gr_released(const glm::ivec2& point, e_input_source input_source, ui32 index);
            void on_gr_moved(const glm::ivec2& point, const glm::ivec2& delta, ui32 index);
            void on_gr_dragged(const glm::ivec2& point, const glm::ivec2& delta,
                               e_input_source input_source, ui32 index);
            
            void on_key_up(i32 key);
            void on_key_down(i32 key);
            
        public:
            
            CTTI_CLASS_GUID(ces_textedit_system, ces_base_system::g_guids_container)
            ces_textedit_system();
            ~ces_textedit_system();
        };
    };
};

#endif
