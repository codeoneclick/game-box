//
//  image_button.cpp
//  gbUI
//
//  Created by serhii.s on 5/10/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "image_button.h"
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
        std::string image_button::k_image_element_name = "image_element";
        std::string image_button::k_foreground_element_name = "foreground_element";
        
        const std::string image_button::k_pressed_state = "pressed";
        const std::string image_button::k_released_state = "released";
        
        image_button::image_button(const scene_fabricator_shared_ptr& fabricator) :
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
                std::static_pointer_cast<gb::sprite>(m_elements[k_image_element_name])->size = size * .8f;
                
                control::set_element_horizontal_aligment(m_elements[k_foreground_element_name], m_horizontal_aligment);
                control::set_element_vertical_aligment(m_elements[k_foreground_element_name], m_vertical_aligment);
                
                control::set_element_horizontal_aligment(m_elements[k_image_element_name], m_horizontal_aligment);
                control::set_element_vertical_aligment(m_elements[k_image_element_name], m_vertical_aligment);
            });
        }
        
        void image_button::setup_components()
        {
            interaction_control::setup_components();
        }
        
        void image_button::create(const std::string& image_filename)
        {
            const auto button_background = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_background_element_name] = button_background;
            add_child(button_background);
            
            const auto button_foreground = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_foreground_element_name] = button_foreground;
            add_child(button_foreground);
            
            const auto button_image = control::get_fabricator()->create_sprite("button_image.xml", image_filename);
            m_elements[k_image_element_name] = button_image;
            add_child(button_image);
            
            set_is_selected(false);
            
            interaction_control::create();
            
            control::set_element_horizontal_aligment(m_elements[k_image_element_name], m_horizontal_aligment);
            control::set_element_vertical_aligment(m_elements[k_image_element_name], m_vertical_aligment);
        }
        
        void image_button::on_touched(const ces_entity_shared_ptr& entity,
                                      const glm::vec2& touch_point,
                                      e_input_source input_source,
                                      e_input_state input_state)
        {
            interaction_control::on_touched(entity, touch_point, input_source, input_state);
            auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
            if(input_state == e_input_state_pressed)
            {
                set_is_selected(true);
                
                auto sound_linkage = m_sounds_linkage.find(k_pressed_state);
                auto sound_component = ces_entity::get_component<gb::al::ces_sound_component>();
                if(sound_component && sound_linkage != m_sounds_linkage.end())
                {
                    sound_component->trigger_sound(sound_linkage->second, false, false);
                }
            }
            else if(input_state == e_input_state_released)
            {
                set_is_selected(false);
                
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
        
        void image_button::on_dragging(const ces_entity_shared_ptr& entity,
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
            
            if(!glm::intersect(bound, touch_point))
            {
                control::set_color(k_background_element_name, glm::u8vec4(255, 0, 0, 255));
                control::set_color(k_foreground_element_name, glm::u8vec4(255, 0, 0, 255));
            }
            else
            {
                 set_is_selected(m_is_selected);
            }
        }
        
        void image_button::set_is_selected(bool value)
        {
            m_is_selected = value;
            set_color(k_background_element_name, m_is_selected ? m_background_color[static_cast<i32>(e_control_state::e_selected)] : m_background_color[static_cast<i32>(e_control_state::e_none)]);
            set_color(k_foreground_element_name, m_is_selected ? m_foreground_color[static_cast<i32>(e_control_state::e_selected)] : m_foreground_color[static_cast<i32>(e_control_state::e_none)]);
            set_color(k_image_element_name, m_is_selected ? m_image_color[static_cast<i32>(e_control_state::e_selected)] : m_image_color[static_cast<i32>(e_control_state::e_none)]);
        }
        
        void image_button::set_image_horizontal_aligment(e_element_horizontal_aligment aligment)
        {
            control::set_element_horizontal_aligment(m_elements[k_image_element_name], aligment);
            m_horizontal_aligment = aligment;
        }
        
        void image_button::set_image_vertical_aligment(e_element_vertical_aligment aligment)
        {
            control::set_element_vertical_aligment(m_elements[k_image_element_name], aligment);
            m_vertical_aligment = aligment;
        }
        
        void image_button::set_background_color(const glm::u8vec4& color, e_control_state state)
        {
            control::set_background_color(color, state);
        }
        
        void image_button::set_foreground_color(const glm::u8vec4& color, e_control_state state)
        {
            control::set_color(k_foreground_element_name, color);
            m_foreground_color[static_cast<i32>(state)] = color;
        }
        
        void image_button::set_image_color(const glm::u8vec4& color, e_control_state state)
        {
            control::set_color(k_image_element_name, color);
            m_image_color[static_cast<i32>(state)] = color;
        }
        
        void image_button::focus(bool value, f32 focus_interval_in_seconds, const on_focus_callback_t& callback)
        {
            control::focus(value, focus_interval_in_seconds, [=](f32 dt) {
                f32 delta = glm::clamp(m_current_focus_interval / m_focus_interval_in_seconds, 0.f, 1.f);
                control::set_color(k_foreground_element_name, glm::mix(m_foreground_color[static_cast<i32>(e_control_state::e_none)],
                                                                       m_foreground_color[static_cast<i32>(e_control_state::e_focused)],
                                                                       delta));
                control::set_color(k_image_element_name, glm::mix(m_image_color[static_cast<i32>(e_control_state::e_none)],
                                                                  m_image_color[static_cast<i32>(e_control_state::e_focused)],
                                                                  delta));
                if (callback)
                {
                    callback(dt);
                }
            });
        }
    }
}