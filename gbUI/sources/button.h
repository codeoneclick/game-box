//
//  button.h
//  gbUI
//
//  Created by sergey.sergeev on 1/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

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
            
            std::string m_dragged_callback_guid;
            
        protected:
            
            t_on_pressed_callback m_on_pressed_callback;
            
            void on_touched(const ces_entity_shared_ptr&,
                            const glm::vec2& touch_point,
                            e_input_source input_source,
                            e_input_state input_state);
            void on_dragged(const ces_entity_shared_ptr&,
                            const glm::vec2& touch_point,
                            e_input_source input_source,
                            e_input_state input_state);
            
            bool m_is_selected;
            
        public:
            
            static const std::string k_pressed_state;
            static const std::string k_released_state;
            
            button(const scene_fabricator_shared_ptr& fabricator);
            ~button();
            
            void setup_components() override;
            
            void create() override;
            
            void set_text(const std::string& text);
            std::string get_text();
            
            void set_text_horizontal_aligment(e_element_horizontal_aligment aligment);
            void set_text_vertical_aligment(e_element_vertical_aligment aligment);
            
            void set_on_pressed_callback(const t_on_pressed_callback& callback);
            bool is_pressed_callback_exist() const;
            
            void set_is_selected(bool value);
        };
    };
};
