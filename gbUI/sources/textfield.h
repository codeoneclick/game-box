//
//  textfield.h
//  gbUI
//
//  Created by serhii serhiiv on 4/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class textfield : public control
        {
        public:
            
            typedef std::function<bool(const std::string&)> text_validator_callback_t;
            
        private:
            
            e_element_horizontal_aligment m_horizontal_aligment;
            e_element_vertical_aligment m_vertical_aligment;
            std::property_rw<bool> focused;
            
        protected:
            
            void on_touched(const ces_entity_shared_ptr&,
                            const glm::vec2& touch_point,
                            e_input_source input_source,
                            e_input_state input_state);
            void on_focus_changed(bool value);
            void on_text_changed(const std::string& symbol);
            void on_backspace();
            
            text_validator_callback_t m_text_validator_callback;
            
        public:
            
            textfield(const scene_fabricator_shared_ptr& fabricator);
            ~textfield();
            
            void setup_components() override;
            
            void create() override;
            
            void set_text(const std::string& text);
            std::string get_text();
            
            void set_text_horizontal_aligment(e_element_horizontal_aligment aligment);
            void set_text_vertical_aligment(e_element_vertical_aligment aligment);
            
            void set_text_validator(const text_validator_callback_t& callback);
            
            void set_font_color(const glm::u8vec4& color);
        };
    };
};
