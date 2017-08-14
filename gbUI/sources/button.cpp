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
#include "ces_bound_touch_component.h"
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
        
        button::button(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_on_pressed_callback(nullptr),
        m_dragged_callback_guid(""),
        m_is_selected(false),
        m_horizontal_aligment(e_element_horizontal_aligment_center),
        m_vertical_aligment(e_element_vertical_aligment_center)
        {
            ces_entity::add_deferred_component_constructor<ces_bound_touch_component>();
            
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
                bound_touch_component->set_bounds(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
                
                std::static_pointer_cast<gb::sprite>(m_elements[k_background_element_name])->size = size;
                std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->font_size = size.y * .75f;
                
                control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
                control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);
            });
        }
        
        button::~button()
        {
            
        }
        
        void button::setup_components()
        {
            control::setup_components();
            auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
            bound_touch_component->enable(e_input_state_pressed, e_input_source_mouse_left, true);
            bound_touch_component->enable(e_input_state_released, e_input_source_mouse_left, true);
            bound_touch_component->add_callback(e_input_state_pressed, std::bind(&button::on_touched, this,
                                                                                 std::placeholders::_1,
                                                                                 std::placeholders::_2,
                                                                                 std::placeholders::_3,
                                                                                 std::placeholders::_4));
            bound_touch_component->add_callback(e_input_state_released, std::bind(&button::on_touched, this,
                                                                                  std::placeholders::_1,
                                                                                  std::placeholders::_2,
                                                                                  std::placeholders::_3,
                                                                                  std::placeholders::_4));
        }
        
        void button::create()
        {
            gb::sprite_shared_ptr button_background = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_background_element_name] = button_background;
            button::add_child(button_background);
            
            gb::label_shared_ptr button_label = control::get_fabricator()->create_label("button_label.xml");
            m_elements[k_label_element_name] = button_label;
            button::add_child(button_label);
            
            button::set_is_selected(false);
            
            control::create();
            
            control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
            control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);
        }
        
        void button::on_touched(const ces_entity_shared_ptr&,
                                const glm::vec2& touch_point,
                                e_input_source input_source,
                                e_input_state input_state)
        {
            auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
            if(input_state == e_input_state_pressed)
            {
                button::set_is_selected(true);
                
                bound_touch_component->enable(e_input_state_dragged, e_input_source_mouse_left, true);
                m_dragged_callback_guid = bound_touch_component->add_callback(e_input_state_dragged, std::bind(&button::on_dragged, this,
                                                                                                               std::placeholders::_1,
                                                                                                               std::placeholders::_2, 
                                                                                                               std::placeholders::_3,
                                                                                                               std::placeholders::_4));
                
                auto sound_linkage = m_sounds_linkage.find(k_pressed_state);
                auto sound_component = ces_entity::get_component<gb::al::ces_sound_component>();
                if(sound_component && sound_linkage != m_sounds_linkage.end())
                {
                    sound_component->trigger_sound(sound_linkage->second);
                }
            }
            else if(input_state == e_input_state_released)
            {
                button::set_is_selected(false);
                
                bound_touch_component->enable(e_input_state_dragged, e_input_source_mouse_left, false);
                bound_touch_component->remove_callback(e_input_state_dragged, m_dragged_callback_guid);
                
                if(m_on_pressed_callback)
                {
                    m_on_pressed_callback(shared_from_this());
                }
                
                auto sound_linkage = m_sounds_linkage.find(k_released_state);
                auto sound_component = ces_entity::get_component<gb::al::ces_sound_component>();
                if(sound_component && sound_linkage != m_sounds_linkage.end())
                {
                    sound_component->trigger_sound(sound_linkage->second);
                }
            }
        }
        
        void button::on_dragged(const ces_entity_shared_ptr&,
                                const glm::vec2& touch_point,
                                e_input_source input_source,
                                e_input_state input_state)
        {
            glm::vec2 size = control::size;
            glm::mat4 mat_m = ces_transformation_extension::get_absolute_transformation_in_ws(shared_from_this());
            glm::vec2 min_bound = glm::transform(glm::vec2(0.f),
                                                 mat_m);
            glm::vec2 max_bound = glm::transform(size,
                                                 mat_m);
            glm::vec4 bound = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
            
            if(!glm::intersect(bound, touch_point))
            {
                auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
                control::set_color(k_background_element_name, glm::u8vec4(255, 0, 0, 255));
                bound_touch_component->enable(e_input_state_dragged, e_input_source_mouse_left, false);
                bound_touch_component->remove_callback(e_input_state_dragged, m_dragged_callback_guid);
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
        
        void button::set_on_pressed_callback(const t_on_pressed_callback& callback)
        {
            m_on_pressed_callback = callback;
        }
        
        bool button::is_pressed_callback_exist() const
        {
            return m_on_pressed_callback != nullptr;
        }
        
        void button::set_is_selected(bool value)
        {
            m_is_selected = value;
            control::set_color(k_background_element_name, m_is_selected ? control::k_light_gray_color : control::k_dark_gray_color);
            control::set_color(k_label_element_name, m_is_selected ? control::k_black_color : control::k_white_color);
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
    }
}
