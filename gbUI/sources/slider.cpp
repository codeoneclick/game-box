//
//  slider.cpp
//  gbUI
//
//  Created by serhii.s on 10/2/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "slider.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_material_component.h"
#include "ces_action_component.h"
#include "ces_transformation_extension.h"
#include "game_command.h"
#include "input_context.h"
#include "glm_extensions.h"

namespace gb
    {
    namespace ui
        {
        slider::slider(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::interaction_control(fabricator)
        {
            ces_entity::add_deferred_component_constructor<ces_bound_touch_2d_component>();
            
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                interaction_control::on_touch_size_changed(m_size);
                
                std::static_pointer_cast<gb::sprite>(m_elements[k_background_element_name])->size = size;
                std::static_pointer_cast<gb::sprite>(m_elements[k_foreground_element_name])->size = glm::vec2(size.x - 4.f, size.y - 4.f);
                std::static_pointer_cast<gb::sprite>(m_elements[k_slider_element_name])->size = glm::vec2(size.y * 1.2f, size.y * 1.2f);
                
                control::set_element_horizontal_aligment(m_elements[k_foreground_element_name], e_element_horizontal_aligment_center);
                control::set_element_vertical_aligment(m_elements[k_foreground_element_name], e_element_vertical_aligment_center);
            });
            
            
        }
        
        slider::~slider()
        {
            
        }
        
        void slider::setup_components()
        {
            interaction_control::setup_components();
        }
        
        void slider::create()
        {
            gb::sprite_shared_ptr background_element = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_background_element_name] = background_element;
            add_child(background_element);
            
            gb::sprite_shared_ptr foreground_element = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_foreground_element_name] = foreground_element;
            add_child(foreground_element);
            
            gb::sprite_shared_ptr slider_element = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_slider_element_name] = slider_element;
            add_child(slider_element);
            
            interaction_control::create();
            
            control::set_color(k_background_element_name, control::k_light_gray_color);
            control::set_color(k_foreground_element_name, control::k_dark_gray_color);
            control::set_color(k_slider_element_name, control::k_white_color);
            
            control::set_element_horizontal_aligment(m_elements[k_foreground_element_name], e_element_horizontal_aligment_center);
            control::set_element_vertical_aligment(m_elements[k_foreground_element_name], e_element_vertical_aligment_center);
        }
        
        void slider::set_callback(const callback_t& callback)
        {
            m_callback = callback;
        }
        
        bool slider::is_callback_exist() const
        {
            return m_callback != nullptr;
        }
        
        void slider::on_touched(const ces_entity_shared_ptr& entity,
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
                on_offset_select(touch_point);
            }
        }
        
        void slider::on_dragging(const ces_entity_shared_ptr& entity,
                                 const glm::vec2& touch_point,
                                 e_input_source input_source,
                                 e_input_state input_state)
        {
            interaction_control::on_dragging(entity, touch_point, input_source, input_state);
            on_offset_select(touch_point);
        }
        
        void slider::set_offset(f32 value)
        {
            m_offset = value;
            m_offset = glm::clamp(m_offset, 0.f, 1.f);
            glm::vec2 current_size = size;
            f32 position_x = current_size.x * m_offset;
            std::static_pointer_cast<gb::sprite>(m_elements[k_slider_element_name])->position = glm::vec2(position_x - current_size.y * 1.2f * .5f,
                                                                                                          -current_size.y * .2f * .5f);
        }
        
        f32 slider::get_offset() const
        {
            return m_offset;
        }
        
        void slider::on_offset_select(const glm::vec2& touch_point)
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
                f32 offset = point.x / size.x;
                set_offset(offset);
                if (m_callback)
                {
                    m_callback(offset);
                }
            }
        }
        
        void slider::set_slider_color(const glm::u8vec4& color, e_control_state state)
        {
            if (m_slider_color[static_cast<i32>(state)] != color)
            {
                control::set_color(k_slider_element_name, color);
                m_slider_color[static_cast<i32>(state)] = color;
            }
        }
        
        void slider::set_foreground_color(const glm::u8vec4& color, e_control_state state)
        {
            if (m_foreground_color[static_cast<i32>(state)] != color)
            {
                control::set_color(k_foreground_element_name, color);
                m_foreground_color[static_cast<i32>(state)] = color;
            }
        }
    }
}
