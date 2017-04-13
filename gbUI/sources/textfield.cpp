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
#include "ces_transformation_extension.h"
#include "game_command.h"
#include "input_context.h"
#include "glm_extensions.h"

namespace gb
{
    namespace ui
    {
        textfield::textfield(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator)
        {
            ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<ces_bound_touch_component>();
            bound_touch_compoent->enable(e_input_state_pressed, e_input_source_mouse_left, true);
            bound_touch_compoent->enable(e_input_state_released, e_input_source_mouse_left, true);
            bound_touch_compoent->add_callback(e_input_state_pressed, std::bind(&textfield::on_touched, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            bound_touch_compoent->add_callback(e_input_state_released, std::bind(&textfield::on_touched, this, std::placeholders::_1,
                                                                                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            ces_entity::add_component(bound_touch_compoent);
            
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
                bound_touch_component->set_frame(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
                
                std::static_pointer_cast<gb::sprite>(m_elements["textfield_background"])->size = size;
                std::static_pointer_cast<gb::label>(m_elements["textfield_label"])->font_size = size.y * .5f;
                
                control::set_element_horizontal_aligment(m_elements["textfield_label"], e_element_horizontal_aligment_center);
                control::set_element_vertical_aligment(m_elements["textfield_label"], e_element_vertical_aligment_center);
                
            });
        }
        
        textfield::~textfield()
        {
            
        }
        
        void textfield::create()
        {
            gb::sprite_shared_ptr button_background = control::get_fabricator()->create_sprite("textfield_background.xml");
            m_elements["textfield_background"] = button_background;
            game_object::add_child(button_background);
            
            gb::label_shared_ptr button_label = control::get_fabricator()->create_label("textfield_label.xml");
            m_elements["textfield_label"] = button_label;
            game_object::add_child(button_label);
            
            control::create();
            
            control::set_color("textfield_background", control::k_dark_gray_color);
            control::set_color("textfield_label", control::k_white_color);
        }
        
        void textfield::on_touched(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state)
        {

        }
        
        void textfield::set_text(const std::string& text)
        {
            std::static_pointer_cast<gb::label>(m_elements["textfield_label"])->text = text;
            
            control::set_element_horizontal_aligment(m_elements["textfield_label"], e_element_horizontal_aligment_center);
            control::set_element_vertical_aligment(m_elements["textfield_label"], e_element_vertical_aligment_center);
        }
        
        std::string textfield::get_text()
        {
            return std::static_pointer_cast<gb::label>(m_elements["textfield_label"])->text;
        }
        
        void textfield::set_text_horizontal_aligment(e_element_horizontal_aligment aligment)
        {
            control::set_element_horizontal_aligment(m_elements["textfield_label"], aligment);
        }
        
        void textfield::set_text_vertical_aligment(e_element_vertical_aligment aligment)
        {
            control::set_element_vertical_aligment(m_elements["textfield_label"], aligment);
        }
    }
}
