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
            
            e_element_horizontal_aligment m_horizontal_aligment;
            e_element_vertical_aligment m_vertical_aligment;
            std::string m_dragged_callback_guid;
            glm::u8vec4 m_background_color;
            
        protected:
            
            t_on_pressed_callback m_on_pressed_callback;
            
            virtual void on_touched(const ces_entity_shared_ptr&,
									const glm::vec2& touch_point,
									e_input_source input_source,
									e_input_state input_state);

            virtual void on_dragged(const ces_entity_shared_ptr&,
									const glm::vec2& touch_point,
									e_input_source input_source,
									e_input_state input_state);
            
            bool m_is_selected;
            
        public:
            
            static const std::string k_pressed_state;
            static const std::string k_released_state;
            
            button(const scene_fabricator_shared_ptr& fabricator);
            virtual ~button();
            
			virtual void setup_components() override;
            
            virtual void create() override;
            
            void set_text(const std::string& text);
            std::string get_text();
            
            void set_text_horizontal_aligment(e_element_horizontal_aligment aligment);
            void set_text_vertical_aligment(e_element_vertical_aligment aligment);
            
            void set_on_pressed_callback(const t_on_pressed_callback& callback);
            bool is_pressed_callback_exist() const;
            
            void set_is_selected(bool value);
            
            void set_background_color(const glm::u8vec4& color) override;
        };
    };
};
