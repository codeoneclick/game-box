//
//  fullscreen_joystick.h
//  gbUI
//
//  Created by serhii serhiiv on 3/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class fullscreen_joystick : public control
        {
        public:
            
            typedef std::function<void(const ces_entity_shared_ptr&, const glm::vec2&, f32)> on_dragging_callback_t;
            typedef std::function<void(const ces_entity_shared_ptr&)> on_end_dragging_callback_t;
            typedef std::function<void(const ces_entity_shared_ptr&)> on_double_tap_callback_t;
            
        private:
            
        protected:
            
            glm::vec2 m_center;
            bool m_is_dragged;
            
            on_dragging_callback_t m_on_dragging_callback;
            on_end_dragging_callback_t m_on_end_dragging_callback;
            on_double_tap_callback_t m_on_double_tap_callback;
            
            void on_dragged(const ces_entity_shared_ptr&,
                            const glm::vec2& touch_point,
                            e_input_source input_source,
                            e_input_state input_state);
            void on_pressed(const ces_entity_shared_ptr&,
                            const glm::vec2& touch_point,
                            e_input_source input_source,
                            e_input_state input_state);
            void on_released(const ces_entity_shared_ptr&,
                             const glm::vec2& touch_point,
                             e_input_source input_source,
                             e_input_state input_state);
            
        public:
            
            fullscreen_joystick(const scene_fabricator_shared_ptr& fabricator);
            ~fullscreen_joystick();
            
            static fullscreen_joystick_shared_ptr construct(const scene_fabricator_shared_ptr& fabricator);
            
            void create();
            
            std::property_rw<glm::vec2> position;
            std::property_rw<glm::vec2> size;
            
            void set_on_dragging_callback(const on_dragging_callback_t& callback);
            void set_on_end_dragging_callback(const on_end_dragging_callback_t& callback);
            void set_on_double_tap_callback(const on_double_tap_callback_t& callback);
        };
    };
};
