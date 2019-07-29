//
//  image_button.h
//  gbUI
//
//  Created by serhii.s on 5/10/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "interaction_control.h"
#include "input_context.h"
#include "button.h"

namespace gb
{
    namespace ui
    {
        class image_button : public interaction_control, public i_button_callbacks
        {
        public:
            
            static std::string k_image_element_name;
            static std::string k_foreground_element_name;
            
        private:
            
            std::array<glm::u8vec4, static_cast<i32>(e_control_state::e_max)> m_foreground_color;
            std::array<glm::u8vec4, static_cast<i32>(e_control_state::e_max)> m_image_color;
            
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
            
            image_button(const scene_fabricator_shared_ptr& fabricator);
            virtual ~image_button() = default;
            
            virtual void setup_components() override;
            
            void create(const std::string& image_filename);
            
            void set_image_horizontal_aligment(e_element_horizontal_aligment aligment);
            void set_image_vertical_aligment(e_element_vertical_aligment aligment);
            
            void set_is_selected(bool value);
            
            void set_background_color(const glm::u8vec4& color, e_control_state state = e_control_state::e_none) override;
            void set_foreground_color(const glm::u8vec4& color, e_control_state state = e_control_state::e_none);
            void set_image_color(const glm::u8vec4& color, e_control_state state = e_control_state::e_none);
            
            void focus(bool value, f32 focus_interval_in_seconds = 1.f, const on_focus_callback_t& callback = nullptr) override;
        };
    };
};

