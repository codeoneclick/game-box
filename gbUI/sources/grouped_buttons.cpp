//
//  grouped_buttons.cpp
//  gbUI
//
//  Created by Serhii Serhiiv on 2/3/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "grouped_buttons.h"
#include "scene_fabricator.h"
#include "ces_bound_touch_component.h"
#include "ces_transformation_component.h"
#include "ces_material_component.h"
#include "sprite.h"
#include "button.h"

namespace gb
{
    namespace ui
    {
        static const std::string k_color_state_uniform = "u_color";
        
        grouped_buttons::grouped_buttons(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_on_pressed_callback(nullptr),
        m_separator_offset(10.f)
        {
            
        }
        
        grouped_buttons::~grouped_buttons()
        {
            
        }
        
        void grouped_buttons::create()
        {
            gb::sprite_shared_ptr grouped_buttons_background =
            control::get_fabricator()->create_sprite("grouped_buttons_background.xml");
            m_elements["grouped_buttons_background"] = grouped_buttons_background;
            ces_entity::add_child(grouped_buttons_background);
            
            ces_material_component* material_component = unsafe_get_material_component(grouped_buttons_background);
            material_component->set_custom_shader_uniform(control::k_dark_gray_color, k_color_state_uniform);
            
            control::create();
        }
        
        void grouped_buttons::set_separator_offset(f32 separator_offset)
        {
            m_separator_offset = separator_offset;
        }
        
        void grouped_buttons::set_size(const glm::vec2& size)
        {
            control::set_size(size);
            std::static_pointer_cast<gb::sprite>(m_elements["grouped_buttons_background"])->set_size(glm::vec2(size.x + m_separator_offset * 2.f, size.y));
        }
        
        void grouped_buttons::set_on_pressed_callback(const t_on_pressed_callback& callback)
        {
            m_on_pressed_callback = callback;
        }
        
        void grouped_buttons::set_data_source(const std::vector<std::string>& data_source)
        {
            f32 offset_x = 0.f;
            f32 size_x = (m_size.x - m_separator_offset * (static_cast<i32>(data_source.size()) - 3)) / data_source.size();
            for(i32 i = 0; i < data_source.size(); ++i)
            {
                button_shared_ptr button = std::make_shared<gb::ui::button>(control::get_fabricator());
                button->create();
                button->set_size(glm::vec2(size_x, m_size.y));
                button->set_position(glm::vec2(offset_x, 0.f));
                button->set_text(data_source[i]);
                button->set_on_pressed_callback(std::bind(&grouped_buttons::on_button_pressed, this, std::placeholders::_1));
                ces_entity::add_child(button);
                m_buttons.push_back(button);
                
                offset_x += size_x + m_separator_offset;
            }
            
            grouped_buttons::on_button_pressed(m_buttons[0]);
        }
        
        void grouped_buttons::on_button_pressed(const ces_entity_shared_ptr& entity)
        {
            for (i32 i = 0; i < m_buttons.size(); ++i)
            {
                m_buttons[i]->set_is_selected(false);
                if(entity == m_buttons[i] && m_on_pressed_callback)
                {
                    m_buttons[i]->set_is_selected(true);
                    m_on_pressed_callback(i, entity);
                }
            }
        }
    }
}