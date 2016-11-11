//
//  joystick.h
//  gbUI
//
//  Created by serhii serhiiv on 10/29/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef joystick_h
#define joystick_h

#include "control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class joystick : public control
        {
        public:
            
            typedef std::function<void(const ces_entity_shared_ptr&, const glm::vec2&)> t_on_dragging_callback;
            typedef std::function<void(const ces_entity_shared_ptr&)> t_on_end_dragging_callback;
            
        private:
            
        protected:
            
            glm::vec2 m_border_size;
            bool m_is_dragged;
            
            t_on_dragging_callback m_on_dragging_callback;
            t_on_end_dragging_callback m_on_end_dragging_callback;
            
            void on_dragged(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state);
            void on_pressed(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state);
            void on_released(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state);
            
        public:
            
            joystick(const scene_fabricator_shared_ptr& fabricator);
            ~joystick();
            
            void create();
            
            void set_size(const glm::vec2& size);
            
            void set_text_horizontal_aligment(e_element_horizontal_aligment aligment);
            void set_text_vertical_aligment(e_element_vertical_aligment aligment);
            
            void set_on_dragging_callback(const t_on_dragging_callback& callback);
            void set_on_end_dragging_callback(const t_on_end_dragging_callback& callback);
        };
    };
};

#endif
