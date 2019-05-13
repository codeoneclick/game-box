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

namespace gb
{
    namespace ui
    {
        class image_button : public interaction_control
        {
        public:
            
            typedef std::function<void(const ces_entity_shared_ptr&)> t_on_pressed_callback;
            static std::string k_image_element_name;
            
        private:
            
            glm::u8vec4 m_background_color = glm::u8vec4(0, 0, 0, 255);
            glm::u8vec4 m_image_color = glm::u8vec4(255, 255, 255, 255);
            
        protected:
            
            t_on_pressed_callback m_on_pressed_callback = nullptr;
            
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
            
            void set_on_pressed_callback(const t_on_pressed_callback& callback);
            bool is_pressed_callback_exist() const;
            
            void set_is_selected(bool value);
            
            void set_background_color(const glm::u8vec4& color) override;
            void set_image_color(const glm::u8vec4& color);
        };
    };
};

