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
        private:
            
        protected:
            
            glm::vec2 m_border_size;
            
            void on_dragged(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state);
            void on_released(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state);
            
        public:
            
            joystick(const scene_fabricator_shared_ptr& fabricator);
            ~joystick();
            
            void create();
            
            void set_size(const glm::vec2& size);
            
            void set_text_horizontal_aligment(e_element_horizontal_aligment aligment);
            void set_text_vertical_aligment(e_element_vertical_aligment aligment);
        };
    };
};

#endif
