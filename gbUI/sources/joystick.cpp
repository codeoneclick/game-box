//
//  joystick.cpp
//  gbUI
//
//  Created by serhii serhiiv on 10/29/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "joystick.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_bound_touch_component.h"
#include "ces_material_component.h"
#include "ces_transformation_extension.h"
#include "game_command.h"
#include "input_context.h"
#include "glm_extensions.h"

namespace gb
{
    namespace ui
    {
        static const std::string k_color_01_uniform = "u_color_01";
        static const std::string k_color_02_uniform = "u_color_02";
        
        static const std::string k_border_color_uniform = "u_border_color";
        static const std::string k_border_size_uniform = "u_border_size";
        
        joystick::joystick(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_border_size(glm::vec2(1.f)),
        m_is_dragged(false),
        m_on_dragging_callback(nullptr),
        m_on_end_dragging_callback(nullptr)
        {
            ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<ces_bound_touch_component>();
            
            bound_touch_compoent->enable(e_input_state_pressed, e_input_source_mouse_left, true);
            bound_touch_compoent->enable(e_input_state_released, e_input_source_mouse_left, true);
            bound_touch_compoent->enable(e_input_state_dragged, e_input_source_mouse_left, true);
            
            bound_touch_compoent->add_callback(e_input_state_pressed, std::bind(&joystick::on_pressed, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            bound_touch_compoent->add_callback(e_input_state_released, std::bind(&joystick::on_released, this, std::placeholders::_1,
                                                                                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            bound_touch_compoent->add_callback(e_input_state_dragged, std::bind(&joystick::on_dragged, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            
            ces_entity::add_component(bound_touch_compoent);
        }
        
        joystick::~joystick()
        {
            
        }
        
        void joystick::create()
        {
            gb::sprite_shared_ptr joystick_background = control::get_fabricator()->create_sprite("joystick_background.xml");
            m_elements["joystick_background"] = joystick_background;
            game_object::add_child(joystick_background);
            
            gb::sprite_shared_ptr joystick_button = control::get_fabricator()->create_sprite("joystick_button.xml");
            m_elements["joystick_button"] = joystick_button;
            game_object::add_child(joystick_button);
            
            control::create();
        }
        
        void joystick::on_pressed(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state)
        {
            
        }
        
        void joystick::on_released(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state)
        {
            control::set_element_horizontal_aligment(m_elements["joystick_button"], e_element_horizontal_aligment_center);
            control::set_element_vertical_aligment(m_elements["joystick_button"], e_element_vertical_aligment_center);
            m_is_dragged = false;
            
            if(m_on_end_dragging_callback)
            {
                m_on_end_dragging_callback(shared_from_this());
            }
        }
        
        void joystick::on_dragged(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state)
        {
            gb::sprite_shared_ptr joystick_button = std::static_pointer_cast<gb::sprite>(m_elements["joystick_button"]);
            glm::vec4 bound = joystick_button->bound;
            glm::mat4 mat_m = ces_transformation_extension::get_absolute_transformation_in_camera_space(joystick_button);
            glm::vec2 min_bound = glm::transform(glm::vec2(bound.x, bound.y),
                                                 mat_m);
            glm::vec2 max_bound = glm::transform(glm::vec2(bound.z, bound.w),
                                                 mat_m);
            bound = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
            
            glm::vec2 parent_offset = glm::vec2(0.f);
            
            ces_entity_shared_ptr parent = joystick_button->parent;
            while (parent) {
                ces_transformation_component_shared_ptr transformation_component = parent->get_component<ces_transformation_component>();
                glm::vec2 parent_position = transformation_component->get_position();
                parent_offset.x += parent_position.x;
                parent_offset.y += parent_position.y;
                parent = parent->parent;
            }
            
            if(glm::intersect(bound, point) && !m_is_dragged)
            {
                m_is_dragged = true;
            }
            
            if(m_is_dragged)
            {
                glm::vec4 element_bound = joystick_button->bound;
                glm::vec2 position = joystick_button->position;
                glm::vec2 delta = position;
                
                position.x = point.x - parent_offset.x - element_bound.z * .5f;
                position.y = point.y - parent_offset.y - element_bound.w * .5f;
                
                joystick_button->position = position;
                
                if(m_on_dragging_callback)
                {
                    glm::vec4 container_bound = control::bound;
                    glm::vec2 center = glm::vec2((container_bound.z - element_bound.z) * .5f,
                                                 (container_bound.w - element_bound.w) * .5f);
                    delta.x = center.x - position.x;
                    delta.y = center.y - position.y;
                    delta *= .01f;
                    m_on_dragging_callback(shared_from_this(), delta);
                }
            }
        }
        
        void joystick::set_size(const glm::vec2& size)
        {
            control::set_size(size);
            
            unsafe_get_bound_touch_component_from_this->set_frame(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
            
            std::static_pointer_cast<gb::sprite>(m_elements["joystick_background"])->size = size;
            std::static_pointer_cast<gb::sprite>(m_elements["joystick_button"])->size = glm::vec2(size.x * .5f, size.y * .5f);
            
            control::set_element_horizontal_aligment(m_elements["joystick_button"], e_element_horizontal_aligment_center);
            control::set_element_vertical_aligment(m_elements["joystick_button"], e_element_vertical_aligment_center);
        }
        
        void joystick::set_on_dragging_callback(const t_on_dragging_callback& callback)
        {
            m_on_dragging_callback = callback;
        }
        
        void joystick::set_on_end_dragging_callback(const t_on_end_dragging_callback& callback)
        {
            m_on_end_dragging_callback = callback;
        }
    }
}