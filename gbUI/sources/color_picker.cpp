//
//  color_picker.cpp
//  gbUI
//
//  Created by serhii.s on 8/10/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "color_picker.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_material_component.h"
#include "ces_transformation_extension.h"
#include "game_command.h"
#include "input_context.h"
#include "glm_extensions.h"
#include "ces_sound_component.h"
#include "texture.h"
#include "std_extensions.h"

namespace gb
{
    namespace ui
    {
        std::string color_picker::k_color_wheel_element_name = "color_wheel_element";
    
        std::pair<f32, f32> color_picker::get_HS_value(const glm::vec2& point, f32 radius)
        {
            f32 dx = (point.x - radius) / radius;
            f32 dy = (point.y - radius) / radius;
            f32 saturation = sqrt(dx * dx + dy * dy);
            f32 hue = 0.f;
            if (saturation != 0.f)
            {
                hue = acos(dx / saturation);
                if (dy < 0.f)
                {
                    hue = M_PI * 2.f - hue;
                }
            }
            
            return std::make_pair(glm::degrees(hue), saturation);
        }
        
        color_picker::color_picker(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::interaction_control(fabricator)
        {
            ces_entity::add_deferred_component_constructor<ces_bound_touch_2d_component>();
            
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                interaction_control::on_touch_size_changed(m_size);
                
                std::static_pointer_cast<gb::sprite>(m_elements[k_background_element_name])->size = size;
                std::static_pointer_cast<gb::sprite>(m_elements[k_color_wheel_element_name])->size = size;
            });
        }
        
        void color_picker::setup_components()
        {
            interaction_control::setup_components();
        }
        
        void color_picker::create()
        {
            const auto background = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_background_element_name] = background;
            add_child(background);
            
            m_rgba = new ui8[m_radius * m_radius * 4];
            
            for (i32 y = 0; y < m_radius; ++y)
            {
                for (i32 x = 0; x < m_radius; ++x)
                {
                    f32 hue = 0.f;
                    f32 saturation = 0.f;
                    f32 r = 0.f; f32 g = 0.f; f32 b = 0.f;
                    f32 a = 0.f;

                    glm::vec2 point = glm::vec2(x, y);
                    const auto value = get_HS_value(point, m_radius / 2.f);
                    hue = value.first;
                    saturation = value.second;
                    if (saturation < 1.f)
                    {
                        if (saturation > .99f)
                        {
                            a = (1.f - saturation) * 100.f;
                        }
                        else
                        {
                            a = 1.f;
                        }
                        std::hsv_to_rgb(r, g, b, hue, saturation, 1.f);
                    }
                    i32 offset = 4 * (x + y * m_radius);
                    m_rgba[offset] = glm::clamp(r * 255.f, 0.f, 255.f);
                    m_rgba[offset + 1] = glm::clamp(g * 255.f, 0.f, 255.f);
                    m_rgba[offset + 2] = glm::clamp(b * 255.f, 0.f, 255.f);
                    m_rgba[offset + 3] = glm::clamp(a * 255.f, 0.f, 255.f);
                }
            }
            
            const auto color_wheel_texture = texture::construct("color_wheel_texture", m_radius, m_radius, gl::constant::rgba_t, m_rgba);
            control::get_fabricator()->get_resource_accessor()->add_custom_resource("color_wheel_texture", color_wheel_texture);
            const auto color_wheel = control::get_fabricator()->create_sprite("color_picker.xml", "color_wheel_texture");
            m_elements[k_color_wheel_element_name] = color_wheel;
            add_child(color_wheel);
            
            interaction_control::create();
        }
        
        void color_picker::on_touched(const ces_entity_shared_ptr& entity,
                                      const glm::vec2& touch_point,
                                      e_input_source input_source,
                                      e_input_state input_state)
        {
            interaction_control::on_touched(entity, touch_point, input_source, input_state);
            auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
            if(input_state == e_input_state_pressed)
            {
               
            }
            else if(input_state == e_input_state_released)
            {
                on_color_select(touch_point);
            }
        }
        
        void color_picker::on_dragging(const ces_entity_shared_ptr& entity,
                                       const glm::vec2& touch_point,
                                       e_input_source input_source,
                                       e_input_state input_state)
        {
            interaction_control::on_dragging(entity, touch_point, input_source, input_state);
            on_color_select(touch_point);
        }
    
        void color_picker::set_callback(const callback_t& callback)
        {
            m_callback = callback;
        }
    
        bool color_picker::is_callback_exist() const
        {
            return m_callback != nullptr;
        }
    
        void color_picker::on_color_select(const glm::vec2& touch_point)
        {
            glm::vec2 size = control::size;
            glm::mat4 mat_m = ces_transformation_extension::get_absolute_transformation_in_ws(shared_from_this());
            glm::vec2 min_bound = glm::transform(glm::vec2(0.f),
                                                 mat_m);
            glm::vec2 max_bound = glm::transform(size,
                                                 mat_m);
            glm::vec4 bound = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
            
            if(glm::intersect(bound, touch_point))
            {
                glm::vec2 point = glm::vec2(touch_point.x - bound.x,
                                            touch_point.y - bound.y);
                i32 offset = 4 * (static_cast<i32>(point.x) + static_cast<i32>(point.y) * m_radius);
                glm::u8vec4 color;
                color.r = m_rgba[offset];
                color.g = m_rgba[offset + 1];
                color.b = m_rgba[offset + 2];
                color.a = m_rgba[offset + 3];
                if (m_callback && (color.r > 0 || color.g > 0 || color.b > 0))
                {
                    m_callback(color);
                }
            }
        }
    }
}
