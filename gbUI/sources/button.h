//
//  button.h
//  gbUI
//
//  Created by sergey.sergeev on 1/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef button_h
#define button_h

#include "control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class button : public control
        {
        public:
            
            typedef std::function<void(const ces_entity_shared_ptr&)> t_on_pressed_callback;
            
        private:
            
        protected:
            
            t_on_pressed_callback m_on_pressed_callback;
            
            e_element_horizontal_aligment m_text_horizontal_aligment;
            e_element_vertical_aligment m_text_vertical_aligment;
            
            void on_text_mesh_updated();
            void on_text_updated();
            
            void on_touched(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_element input_element, e_input_state input_state);
            void on_dragged(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_element input_element, e_input_state input_state);
            
        public:
            
            button(const scene_fabricator_shared_ptr& fabricator);
            ~button();
            
            void create();
            
            void set_size(const glm::vec2& size);
            
            void set_text(const std::string& text);
            std::string get_text();
            
            void set_text_horizontal_aligment(e_element_horizontal_aligment aligment);
            void set_text_vertical_aligment(e_element_vertical_aligment aligment);
            
            void set_on_pressed_callback(const t_on_pressed_callback& callback);
        };
    };
};

#endif
