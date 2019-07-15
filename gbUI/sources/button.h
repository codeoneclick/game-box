//
//  button.h
//  gbUI
//
//  Created by sergey.sergeev on 1/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "interaction_control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        
        class i_button_callbacks
        {
            public:
            
            typedef std::function<void(const ces_entity_shared_ptr&)> t_on_pressed_callback;
            
            private:
            
            protected:
            
            t_on_pressed_callback m_on_pressed_callback = nullptr;
            
            public:
            
            virtual void set_on_pressed_callback(const t_on_pressed_callback& callback);
            virtual bool is_pressed_callback_exist() const;
            
        };
        
        class button : public interaction_control, public i_button_callbacks
        {
            
        private:
            
            glm::u8vec4 m_background_color;
            glm::u8vec4 m_label_color;
            
        protected:
            
            virtual void on_touched(const ces_entity_shared_ptr&,
									const glm::vec2& touch_point,
									e_input_source input_source,
									e_input_state input_state) override;

            virtual void on_dragging(const ces_entity_shared_ptr&,
									 const glm::vec2& touch_point,
									 e_input_source input_source,
									 e_input_state input_state) override;
            
            bool m_is_selected;

			e_element_horizontal_aligment m_horizontal_aligment;
			e_element_vertical_aligment m_vertical_aligment;
            
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
            
            void set_is_selected(bool value);
            
            void set_background_color(const glm::u8vec4& color) override;
            void set_text_color(const glm::u8vec4& color);
        };
    };
};
