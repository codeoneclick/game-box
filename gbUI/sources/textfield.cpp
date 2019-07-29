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
#include "ces_bound_touch_2d_component.h"
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
        gb::ui::interaction_control(fabricator),
        m_horizontal_aligment(e_element_horizontal_aligment_center),
        m_vertical_aligment(e_element_vertical_aligment_center),
        m_text_validator_callback(nullptr)
        {
            ces_entity::add_deferred_component_constructor<ces_bound_touch_2d_component>();
            ces_entity::add_deferred_component_constructor<ces_textedit_component>();
            
            size.setter([=](const glm::vec2& size)
			{
                m_size = size;
               
				interaction_control::on_touch_size_changed(m_size);
                
                std::static_pointer_cast<gb::sprite>(m_elements[k_background_element_name])->size = size;
				std::static_pointer_cast<gb::sprite>(m_elements[k_foreground_element_name])->size = glm::vec2(size.x - 4.f, size.y - 4.f);
                std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->font_size = size.y * .75f;
                
                control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
                control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);

				control::set_element_horizontal_aligment(m_elements[k_foreground_element_name], e_element_horizontal_aligment_center);
				control::set_element_vertical_aligment(m_elements[k_foreground_element_name], e_element_vertical_aligment_center);
            });
            
            focused.setter([=](bool value) 
			{
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
        
        void textfield::setup_components()
        {
			interaction_control::setup_components();

            auto textedit_component = ces_entity::get_component<ces_textedit_component>();
            textedit_component->set_on_focus_changed_callback(std::bind(&textfield::on_focus_changed, this, std::placeholders::_1));
            textedit_component->set_on_text_changed_callback(std::bind(&textfield::on_text_changed, this, std::placeholders::_1));
            textedit_component->set_on_backspace_callback(std::bind(&textfield::on_backspace, this));
        }
        
        void textfield::create()
        {
            gb::sprite_shared_ptr background_element = control::get_fabricator()->create_sprite("textfield_background.xml");
            m_elements[k_background_element_name] = background_element;
            textfield::add_child(background_element);

			gb::sprite_shared_ptr foreground_element = control::get_fabricator()->create_sprite("textfield_background.xml");
			m_elements[k_foreground_element_name] = foreground_element;
			textfield::add_child(foreground_element);
            
            gb::label_shared_ptr button_label = control::get_fabricator()->create_label_2d("textfield_label.xml");
            m_elements[k_label_element_name] = button_label;
            textfield::add_child(button_label);
            
			interaction_control::create();
            
            control::set_color(k_background_element_name, control::k_light_gray_color);
			control::set_color(k_foreground_element_name, control::k_dark_gray_color);
            control::set_color(k_label_element_name, control::k_black_color);
            
            control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
            control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);

			control::set_element_horizontal_aligment(m_elements[k_foreground_element_name], e_element_horizontal_aligment_center);
			control::set_element_vertical_aligment(m_elements[k_foreground_element_name], e_element_vertical_aligment_center);
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
        
        void textfield::set_text_color(const glm::u8vec4& color, e_control_state state)
        {
            control::set_color(k_label_element_name, color);
            m_text_color[static_cast<i32>(state)] = color;
        }
        
        void textfield::set_font_size(const f32 font_size)
        {
            std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->font_size = font_size;
        }
        
        void textfield::set_font_name(const std::string& font_name)
        {
            std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->font_name = font_name;
        }
        
        void textfield::on_focus_changed(bool value)
        {
            textfield::focused = value;
        }
        
        void textfield::on_text_changed(const std::string& symbol)
        {
			if (m_is_editable)
			{
				bool is_change_valid = true;
				if (m_text_validator_callback)
				{
					is_change_valid = m_text_validator_callback(symbol);
				}
				if (is_change_valid)
				{
					std::string current_text = std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->text;
					current_text.append(symbol);
					textfield::set_text(current_text);
				}
			}
        }
        
        void textfield::on_backspace()
        {
			if (m_is_editable)
			{
				std::string current_text = std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->text;
				current_text.pop_back();
				textfield::set_text(current_text);
			}
        }
        
        void textfield::set_text_validator(const text_validator_callback_t& callback)
        {
            m_text_validator_callback = callback;
        }
        
        void textfield::set_multiline(bool value)
        {
            m_elements[k_label_element_name]->as<gb::label>()->set_multiline(value, m_size.x * .9f);
            control::set_element_horizontal_aligment(m_elements[k_label_element_name], m_horizontal_aligment);
            control::set_element_vertical_aligment(m_elements[k_label_element_name], m_vertical_aligment);
        }

		void textfield::set_editable(bool value)
		{
			m_is_editable = value;
		}

		bool textfield::is_editable() const 
		{
			return m_is_editable;
		}
        
        void textfield::set_visible_edges(bool value)
        {
            m_elements[k_background_element_name]->visible = value;
            m_elements[k_foreground_element_name]->visible = value;
        }
        
        glm::vec2 textfield::get_content_size()
        {
            auto transformation_component = get_component<ces_transformation_component>();
            return std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->get_content_size() * transformation_component->as_2d()->get_scale();
        }
        
        void textfield::set_font_mode(ces_font_component::e_font_mode font_mode)
        {
            std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->font_mode = font_mode;
        }
        
        void textfield::set_foreground_color(const glm::u8vec4& color, e_control_state state)
        {
            control::set_color(k_foreground_element_name, color);
            m_foreground_color[static_cast<i32>(state)] = color;
        }
        
        void textfield::focus(bool value, f32 focus_interval_in_seconds, const on_focus_callback_t& callback)
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
        
        void textfield::disable(bool value)
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
