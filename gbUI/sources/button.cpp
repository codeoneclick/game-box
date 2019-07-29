//
//  button.cpp
//  gbUI
//
//  Created by sergey.sergeev on 1/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "button.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "label.h"
#include "ces_font_component.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_material_component.h"
#include "ces_transformation_extension.h"
#include "game_command.h"
#include "input_context.h"
#include "glm_extensions.h"
#include "ces_sound_component.h"

namespace gb
{
    namespace ui
    {
        const std::string button::k_pressed_state = "pressed";
        const std::string button::k_released_state = "released";
        
        std::string button::k_foreground_element_name = "foreground_element";
        
        void i_button_callbacks::set_on_pressed_callback(const t_on_pressed_callback& callback)
        {
            m_on_pressed_callback = callback;
        }
        
        bool i_button_callbacks::is_pressed_callback_exist() const
        {
            return m_on_pressed_callback != nullptr;
        }
        
        button::button(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::interaction_control(fabricator),
        m_is_selected(false),
        m_horizontal_aligment(e_element_horizontal_aligment_center),
        m_vertical_aligment(e_element_vertical_aligment_center)
        {
            ces_entity::add_deferred_component_constructor<ces_bound_touch_2d_component>();
            
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
				interaction_control::on_touch_size_changed(m_size);

                std::static_pointer_cast<gb::sprite>(m_elements[k_background_element_name])->size = size;
                std::static_pointer_cast<gb::sprite>(m_elements[k_foreground_element_name])->size = size * .9f;
                std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->font_size = size.y * .75f;
                
                control::set_element_horizontal_aligment(m_elements[k_foreground_element_name], m_horizontal_aligment);
                control::set_element_vertical_aligment(m_elements[k_foreground_element_name], m_vertical_aligment);
                
                control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
                control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);
            });
        }
        
        button::~button()
        {
            
        }
        
        void button::setup_components()
        {
			interaction_control::setup_components();
        }
        
        void button::create()
        {
            gb::sprite_shared_ptr button_background = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_background_element_name] = button_background;
            button::add_child(button_background);
            
            const auto button_foreground = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_foreground_element_name] = button_foreground;
            add_child(button_foreground);
            
            gb::label_shared_ptr button_label = control::get_fabricator()->create_label_2d("button_label.xml");
            m_elements[k_label_element_name] = button_label;
            button::add_child(button_label);
            
            button::set_is_selected(false);
            
            interaction_control::create();
            
            control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
            control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);
        }
        
        void button::on_touched(const ces_entity_shared_ptr& entity,
                                const glm::vec2& touch_point,
                                e_input_source input_source,
                                e_input_state input_state)
        {
			interaction_control::on_touched(entity, touch_point, input_source, input_state);
            auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
            if(input_state == e_input_state_pressed)
            {
                button::set_is_selected(true);
                
                auto sound_linkage = m_sounds_linkage.find(k_pressed_state);
                auto sound_component = ces_entity::get_component<gb::al::ces_sound_component>();
                if(sound_component && sound_linkage != m_sounds_linkage.end())
                {
                    sound_component->trigger_sound(sound_linkage->second, false, false);
                }
            }
            else if(input_state == e_input_state_released)
            {
                button::set_is_selected(false);
                
                if (m_on_pressed_callback)
                {
                    m_on_pressed_callback(shared_from_this());
                }

                auto sound_linkage = m_sounds_linkage.find(k_released_state);
                auto sound_component = ces_entity::get_component<gb::al::ces_sound_component>();
                if(sound_component && sound_linkage != m_sounds_linkage.end())
                {
                    sound_component->trigger_sound(sound_linkage->second, false, false);
                }
            }
        }
        
        void button::on_dragging(const ces_entity_shared_ptr& entity,
                                 const glm::vec2& touch_point,
                                 e_input_source input_source,
                                 e_input_state input_state)
        {
			interaction_control::on_dragging(entity, touch_point, input_source, input_state);
			
            glm::vec2 size = control::size;
            glm::mat4 mat_m = ces_transformation_extension::get_absolute_transformation_in_ws(shared_from_this());
            glm::vec2 min_bound = glm::transform(glm::vec2(0.f),
                                                 mat_m);
            glm::vec2 max_bound = glm::transform(size,
                                                 mat_m);
            glm::vec4 bound = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
            
            if (!glm::intersect(bound, touch_point))
            {
                control::set_color(k_background_element_name, glm::u8vec4(255, 0, 0, 255));
            }
			else 
			{
                set_is_selected(m_is_selected);
			}
        }
        
        void button::set_text(const std::string& text)
        {
            std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->text = text;
            
            control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
            control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);
            
            button::set_is_selected(false);
        }
        
        std::string button::get_text()
        {
            return std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->text;
        }
        
        void button::set_is_selected(bool value)
        {
            m_is_selected = value;
            set_color(k_background_element_name, m_is_selected ? m_background_color[static_cast<i32>(e_control_state::e_selected)] : m_background_color[static_cast<i32>(e_control_state::e_none)]);
            set_color(k_foreground_element_name, m_is_selected ? m_foreground_color[static_cast<i32>(e_control_state::e_selected)] : m_foreground_color[static_cast<i32>(e_control_state::e_none)]);
            set_color(k_label_element_name, m_is_selected ? m_text_color[static_cast<i32>(e_control_state::e_selected)] : m_text_color[static_cast<i32>(e_control_state::e_none)]);
        }
        
        void button::set_text_horizontal_aligment(e_element_horizontal_aligment aligment)
        {
            control::set_element_horizontal_aligment(m_elements[k_label_element_name], aligment);
            m_horizontal_aligment = aligment;
        }
        
        void button::set_text_vertical_aligment(e_element_vertical_aligment aligment)
        {
            control::set_element_vertical_aligment(m_elements[k_label_element_name], aligment);
            m_vertical_aligment = aligment;
        }
        
        void button::set_foreground_color(const glm::u8vec4& color, e_control_state state)
        {
            control::set_color(k_foreground_element_name, color);
            m_foreground_color[static_cast<i32>(state)] = color;
        }
        
        void button::set_text_color(const glm::u8vec4& color, e_control_state state)
        {
            control::set_color(k_label_element_name, color);
            m_text_color[static_cast<i32>(state)] = color;
        }
        
        void button::set_font_size(const f32 font_size)
        {
            std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->font_size = font_size;
        }
        
        void button::focus(bool value, f32 focus_interval_in_seconds, const on_focus_callback_t& callback)
        {
            control::focus(value, focus_interval_in_seconds, [=](f32 dt) {
                f32 delta = glm::clamp(m_current_focus_interval / m_focus_interval_in_seconds, 0.f, 1.f);
                control::set_color(k_foreground_element_name, glm::mix(m_foreground_color[static_cast<i32>(e_control_state::e_disabled)],
                                                                       m_foreground_color[static_cast<i32>(e_control_state::e_focused)],
                                                                       delta));
                control::set_color(k_label_element_name, glm::mix(m_text_color[static_cast<i32>(e_control_state::e_disabled)],
                                                                  m_text_color[static_cast<i32>(e_control_state::e_focused)],
                                                                  delta));
                if (callback)
                {
                    callback(dt);
                }
            });
        }
        
        void button::disable(bool value)
        {
            interaction_control::disable(value);
            if (value)
            {
                control::set_color(k_foreground_element_name, m_foreground_color[static_cast<i32>(e_control_state::e_disabled)]);
                control::set_color(k_label_element_name, m_text_color[static_cast<i32>(e_control_state::e_disabled)]);
            }
            else
            {
                control::set_color(k_foreground_element_name, m_foreground_color[static_cast<i32>(e_control_state::e_none)]);
                control::set_color(k_label_element_name, m_text_color[static_cast<i32>(e_control_state::e_none)]);
            }
        }
    }
}
