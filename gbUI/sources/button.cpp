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
#include "ces_material_component.h"
#include "game_command.h"
#include "input_context.h"
#include "glm_extensions.h"

namespace gb
{
    namespace ui
    {
        static const std::string k_color_state_uniform = "u_color";
        
        button::button(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_text_horizontal_aligment(e_element_horizontal_aligment_left),
        m_text_vertical_aligment(e_element_vertical_aligment_top)
        {
            ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<ces_bound_touch_component>();
            bound_touch_compoent->enable(e_input_state_pressed, true);
            bound_touch_compoent->enable(e_input_state_released, true);
            bound_touch_compoent->set_callback(e_input_state_pressed, std::bind(&button::on_touched, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            bound_touch_compoent->set_callback(e_input_state_released, std::bind(&button::on_touched, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
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
            
            ces_material_component* material_component = unsafe_get_material_component(button_background);
            material_component->set_custom_shader_uniform(glm::vec4(1.f, 0.f, 0.f, 1.f), k_color_state_uniform);
            button_label->set_text_color(glm::vec4(0.f, 0.f, 0.f, 1.f));
        }
        
        void button::on_touched(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_element input_element, e_input_state input_state)
        {
            if(input_state == e_input_state_pressed)
            {
                ces_material_component* material_component = unsafe_get_material_component(m_elements["button_background"]);
                material_component->set_custom_shader_uniform(glm::vec4(0.f, 1.f, 0.f, 1.f), k_color_state_uniform);
                unsafe_get_bound_touch_component_from_this->enable(e_input_state_dragged, true);
                unsafe_get_bound_touch_component_from_this->set_callback(e_input_state_dragged, std::bind(&button::on_dragged, this, std::placeholders::_1,
                                                                                                          std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            }
            else if(input_state == e_input_state_released)
            {
                ces_material_component* material_component = unsafe_get_material_component(m_elements["button_background"]);
                material_component->set_custom_shader_uniform(glm::vec4(1.f, 0.f, 0.f, 1.f), k_color_state_uniform);
                unsafe_get_bound_touch_component_from_this->enable(e_input_state_dragged, false);
                unsafe_get_bound_touch_component_from_this->set_callback(e_input_state_dragged, nullptr);
            }
        }
        
        void button::on_dragged(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_element input_element, e_input_state input_state)
        {
            glm::vec4 bound = control::get_bound();
            glm::mat4 mat_m = game_object::get_cs_mat_m();
            glm::vec2 min_bound = glm::transform(glm::vec2(bound.x, bound.y),
                                                 mat_m);
            glm::vec2 max_bound = glm::transform(glm::vec2(bound.z, bound.w),
                                                 mat_m);
            bound = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
            
            if(!glm::intersect(bound, point))
            {
                ces_material_component* material_component = unsafe_get_material_component(m_elements["button_background"]);
                material_component->set_custom_shader_uniform(glm::vec4(1.f, 0.f, 0.f, 1.f), k_color_state_uniform);
                unsafe_get_bound_touch_component_from_this->enable(e_input_state_dragged, false);
                unsafe_get_bound_touch_component_from_this->set_callback(e_input_state_dragged, nullptr);
            }
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