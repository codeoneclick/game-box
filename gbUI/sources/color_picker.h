//
//  color_picker.hpp
//  gbUI
//
//  Created by serhii.s on 8/10/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "interaction_control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class color_picker : public interaction_control
        {
            public:
            
            static std::string k_color_wheel_element_name;
            typedef std::function<void(const glm::u8vec4)> callback_t;
            
            private:
            
            static std::pair<f32, f32> get_HS_value(const glm::vec2& point, f32 radius);
            
            ui8* m_rgba = nullptr;
            texture_shared_ptr m_color_wheel_texture = nullptr;
            
            i32 m_radius = 144;
            f32 m_brightness = 1.f;
            
            callback_t m_callback = nullptr;
            
            void generate_color_wheel();
            void on_color_select(const glm::vec2& point);
            
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
            
            color_picker(const scene_fabricator_shared_ptr& fabricator);
            virtual ~color_picker() = default;
            
            virtual void setup_components() override;
            
            void create() override;
            
            void set_callback(const callback_t& callback);
            bool is_callback_exist() const;
            
            void set_brightness(f32 value);
        };
    };
};


