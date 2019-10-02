//
//  slider.h
//  gbUI
//
//  Created by serhii.s on 10/2/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "interaction_control.h"
#include "ces_font_component.h"

namespace gb
{
    namespace ui
    {
        class slider : public interaction_control
        {
        public:
            
            std::string k_foreground_element_name = "ui_foreground_element";
            std::string k_slider_element_name = "ui_slider_element";
            typedef std::function<void(f32)> callback_t;
            
        private:
            
            f32 m_offset = 0.f;
            callback_t m_callback = nullptr;
            
            std::array<glm::u8vec4, static_cast<i32>(e_control_state::e_max)> m_foreground_color;
            std::array<glm::u8vec4, static_cast<i32>(e_control_state::e_max)> m_slider_color;
            
            void on_offset_select(const glm::vec2& point);
            
        protected:

            virtual void on_touched(const ces_entity_shared_ptr&,
                                    const glm::vec2& touch_point,
                                    e_input_source input_source,
                                    e_input_state input_state) override;
            
            virtual void on_dragging(const ces_entity_shared_ptr&,
                                     const glm::vec2& touch_point,
                                     e_input_source input_source,
                                     e_input_state input_state) override;
            
        public:
            
            slider(const scene_fabricator_shared_ptr& fabricator);
            ~slider();
            
            void setup_components() override;
            
            void create() override;
            
            void set_offset(f32 value);
            f32 get_offset() const;
            
            void set_callback(const callback_t& callback);
            bool is_callback_exist() const;
            
            void set_slider_color(const glm::u8vec4& color, e_control_state state = e_control_state::e_none);
            void set_foreground_color(const glm::u8vec4& color, e_control_state state = e_control_state::e_none);
        };
    };
};


