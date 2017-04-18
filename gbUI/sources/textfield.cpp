//
//  textfield.cpp
//  gbUI
//
//  Created by serhii serhiiv on 4/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "textfield.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "label.h"
#include "ces_font_component.h"
#include "ces_bound_touch_component.h"
#include "ces_textedit_component.h"
#include "ces_material_component.h"
#include "ces_action_component.h"
#include "ces_transformation_extension.h"
#include "game_command.h"
#include "input_context.h"
#include "glm_extensions.h"

#define k_blinking_interval_delta .1f

namespace gb
{
    namespace ui
    {
        textfield::textfield(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_horizontal_aligment(e_element_horizontal_aligment_center),
        m_vertical_aligment(e_element_vertical_aligment_center),
        m_text_validator_callback(nullptr)
        {
            ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<ces_bound_touch_component>();
            bound_touch_compoent->enable(e_input_state_pressed, e_input_source_mouse_left, true);
            bound_touch_compoent->enable(e_input_state_released, e_input_source_mouse_left, true);
            bound_touch_compoent->add_callback(e_input_state_pressed, std::bind(&textfield::on_touched, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            bound_touch_compoent->add_callback(e_input_state_released, std::bind(&textfield::on_touched, this, std::placeholders::_1,
                                                                                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            ces_entity::add_component(bound_touch_compoent);
            
            auto textedit_component = std::make_shared<ces_textedit_component>();
            textedit_component->set_on_focus_changed_callback(std::bind(&textfield::on_focus_changed, this, std::placeholders::_1));
            textedit_component->set_on_text_changed_callback(std::bind(&textfield::on_text_changed, this, std::placeholders::_1));
            textedit_component->set_on_backspace_callback(std::bind(&textfield::on_backspace, this));
            textfield::add_component(textedit_component);
            
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
                bound_touch_component->set_frame(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
                
                std::static_pointer_cast<gb::sprite>(m_elements[k_background_element_name])->size = size;
                std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->font_size = size.y * .5f;
                
                control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
                control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);
            });
            
            focused.setter([=](bool value) {
                auto label = std::static_pointer_cast<gb::label>(m_elements[k_label_element_name]);
                if(!value)
                {
                    glm::u8vec4 color = label->font_color;
                    color.w = 255;
                    label->font_color = color;
                    textfield::remove_component(ces_action_component::class_guid());
                }
                else
                {
                    textfield::remove_component(ces_action_component::class_guid());
                    auto action_component = std::make_shared<gb::ces_action_component>();
                    action_component->set_update_callback([=](const ces_entity_shared_ptr&, f32 dt){
                        static f32 angle = 0.f;
                        angle += k_blinking_interval_delta;
                        f32 delta = (cosf(angle) + 1.f) / 2.f;
                        glm::u8vec4 color = label->font_color;
                        color.w = glm::mix(64, 255, delta);
                        label->font_color = color;
                    });
                    textfield::add_component(action_component);
                }
            });
        }
        
        textfield::~textfield()
        {
            
        }
        
        void textfield::create()
        {
            gb::sprite_shared_ptr button_background = control::get_fabricator()->create_sprite("textfield_background.xml");
            m_elements[k_background_element_name] = button_background;
            game_object::add_child(button_background);
            
            gb::label_shared_ptr button_label = control::get_fabricator()->create_label("textfield_label.xml");
            m_elements[k_label_element_name] = button_label;
            game_object::add_child(button_label);
            
            control::create();
            
            control::set_color(k_background_element_name, control::k_dark_gray_color);
            control::set_color(k_label_element_name, control::k_white_color);
            
            control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
            control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);
        }
        
        void textfield::on_touched(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state)
        {

        }
        
        void textfield::set_text(const std::string& text)
        {
            std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->text = text;
            control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
        }
        
        std::string textfield::get_text()
        {
            return std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->text;
        }
        
        void textfield::set_text_horizontal_aligment(e_element_horizontal_aligment aligment)
        {
            control::set_element_horizontal_aligment(m_elements[k_label_element_name], aligment);
            m_horizontal_aligment = aligment;
        }
        
        void textfield::set_text_vertical_aligment(e_element_vertical_aligment aligment)
        {
            control::set_element_vertical_aligment(m_elements[k_label_element_name], aligment);
            m_vertical_aligment = aligment;
        }
        
        void textfield::set_font_color(const glm::u8vec4& color)
        {
            control::set_color(k_label_element_name, color);
        }
        
        void textfield::set_background_color(const glm::u8vec4& color)
        {
             control::set_color(k_background_element_name, color);
        }
        
        void textfield::on_focus_changed(bool value)
        {
            textfield::focused = value;
        }
        
        void textfield::on_text_changed(const std::string& symbol)
        {
            bool is_change_valid = true;
            if(m_text_validator_callback)
            {
                is_change_valid = m_text_validator_callback(symbol);
            }
            if(is_change_valid)
            {
                std::string current_text = std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->text;
                current_text.append(symbol);
                textfield::set_text(current_text);
            }
        }
        
        void textfield::on_backspace()
        {
            std::string current_text = std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->text;
            current_text.pop_back();
            textfield::set_text(current_text);
        }
        
        void textfield::set_text_validator(const text_validator_callback_t& callback)
        {
            m_text_validator_callback = callback;
        }
    }
}
