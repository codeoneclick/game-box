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
#include "ces_text_component.h"
#include "ces_bound_touch_component.h"
#include "game_command.h"

namespace gb
{
    namespace ui
    {
        button::button(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_text_horizontal_aligment(e_element_horizontal_aligment_left),
        m_text_vertical_aligment(e_element_vertical_aligment_top)
        {
            ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<ces_bound_touch_component>();
            ces_entity::add_component(bound_touch_compoent);
        }
        
        button::~button()
        {
            
        }
        
        void button::create()
        {
            gb::sprite_shared_ptr button_background = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements["button_background"] = button_background;
            ces_entity::add_child(button_background);
            
            gb::label_shared_ptr button_label = control::get_fabricator()->create_label("button_label.xml");
            m_elements["button_label"] = button_label;
            ces_entity::add_child(button_label);
            
            gb::game_command_i_shared_ptr command = std::make_shared<gb::game_command<geometry_on_mesh_updated::t_command>>(std::bind(&button::on_text_mesh_updated, this));
            button_label->get_component(e_ces_component_type_geometry)->add_event_listener(geometry_on_mesh_updated::guid, command);
            
            command = std::make_shared<gb::game_command<text_on_text_updated::t_command>>(std::bind(&button::on_text_updated, this));
            button_label->get_component(e_ces_component_type_text)->add_event_listener(text_on_text_updated::guid, command);
        }
        
        void button::on_text_mesh_updated()
        {
            control::set_element_horizontal_aligment(m_elements["button_label"], m_text_horizontal_aligment);
            control::set_element_vertical_aligment(m_elements["button_label"], m_text_vertical_aligment);
        }
        
        void button::on_text_updated()
        {
            control::set_element_horizontal_aligment(m_elements["button_label"], m_text_horizontal_aligment);
            control::set_element_vertical_aligment(m_elements["button_label"], m_text_vertical_aligment);
        }
        
        void button::set_size(const glm::vec2& size)
        {
            m_size = size;
            
            unsafe_get_bound_touch_component_from_this->set_frame(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
            
            std::static_pointer_cast<gb::sprite>(m_elements["button_background"])->set_size(size);
            std::static_pointer_cast<gb::label>(m_elements["button_label"])->set_font_height(size.y * .5f);
        }
        
        glm::vec2 button::get_size() const
        {
            return m_size;
        }
        
        void button::set_text(const std::string& text)
        {
            std::static_pointer_cast<gb::label>(m_elements["button_label"])->set_text(text);
            button::set_text_horizontal_aligment(e_element_horizontal_aligment_center);
            button::set_text_vertical_aligment(e_element_vertical_aligment_center);
        }
        
        std::string button::get_text()
        {
            return std::static_pointer_cast<gb::label>(m_elements["button_label"])->get_text();
        }
        
        void button::set_text_horizontal_aligment(e_element_horizontal_aligment aligment)
        {
            m_text_horizontal_aligment = aligment;
        }
        
        void button::set_text_vertical_aligment(e_element_vertical_aligment aligment)
        {
            m_text_vertical_aligment = aligment;
        }
    }
}