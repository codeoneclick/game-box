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

#define k_extended_touch_zone 2.f

namespace gb
{
    namespace ui
    {
        
        joystick::joystick(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_is_dragged(false),
        m_on_dragging_callback(nullptr),
        m_on_end_dragging_callback(nullptr),
        m_on_double_tap_callback(nullptr)
        {
            ces_bound_touch_component_shared_ptr bound_touch_component = std::make_shared<ces_bound_touch_component>();
            
            bound_touch_component->enable(e_input_state_pressed, e_input_source_mouse_left, true);
            bound_touch_component->enable(e_input_state_released, e_input_source_mouse_left, true);
            bound_touch_component->enable(e_input_state_dragged, e_input_source_mouse_left, true);
            
            bound_touch_component->add_callback(e_input_state_pressed, std::bind(&joystick::on_pressed, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            bound_touch_component->add_callback(e_input_state_released, std::bind(&joystick::on_released, this, std::placeholders::_1,
                                                                                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            bound_touch_component->add_callback(e_input_state_dragged, std::bind(&joystick::on_dragged, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
            
            ces_entity::add_component(bound_touch_component);
            
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                bound_touch_component->set_frame(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
                
                std::static_pointer_cast<gb::sprite>(m_elements["joystick_background"])->size = size;
                std::static_pointer_cast<gb::sprite>(m_elements["joystick_button"])->size = size * .5f;
                
                control::set_element_horizontal_aligment(m_elements["joystick_button"], e_element_horizontal_aligment_center);
                control::set_element_vertical_aligment(m_elements["joystick_button"], e_element_vertical_aligment_center);
            });
        }
        
        joystick::~joystick()
        {
            
        }
        
        void joystick::create()
        {
            gb::sprite_shared_ptr joystick_background = control::get_fabricator()->create_sprite("joystick_background.xml");
            m_elements["joystick_background"] = joystick_background;
            joystick::add_child(joystick_background);
            
            gb::sprite_shared_ptr joystick_button = control::get_fabricator()->create_sprite("joystick_button.xml");
            m_elements["joystick_button"] = joystick_button;
            joystick::add_child(joystick_button);
            
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
            gb::sprite_shared_ptr joystick_background = std::static_pointer_cast<gb::sprite>(m_elements["joystick_background"]);
            glm::vec2 joystick_button_size = joystick_button->size;
            m_is_dragged = true;

            //auto bound_touch_component = joystick::get_component<ces_bound_touch_component>();
            /*glm::mat4 mat_m = ces_transformation_extension::get_absolute_transformation_in_ws(joystick_button);
            glm::vec2 min_bound = glm::transform(glm::vec2(bound_touch_component->get_frame().x * .5f, bound_touch_component->get_frame().y * .5f),
                                                 mat_m);
            glm::vec2 max_bound = glm::transform(glm::vec2(bound_touch_component->get_frame().z * .5f, bound_touch_component->get_frame().w * .5f),
                                                 mat_m);
            glm::vec4 element_bound = glm::vec4(min_bound.x, min_bound.y,
                                                max_bound.x, max_bound.y);*/
            
            glm::vec2 parent_offset = glm::vec2(0.f);
            
            /*if(glm::intersect(element_bound, point) && !m_is_dragged)
            {
                m_is_dragged = true;
            }*/
            
            ces_entity_shared_ptr parent = joystick_button->parent;
            while (parent) {
                auto transformation_component = parent->get_component<ces_transformation_2d_component>();
                glm::vec2 parent_position = transformation_component->get_position();
                parent_offset.x += parent_position.x;
                parent_offset.y += parent_position.y;
                parent = parent->parent;
            }
            
            if(m_is_dragged)
            {
                glm::vec2 position = joystick_button->position;
                glm::vec2 delta = position;
                
                position.x = point.x - parent_offset.x - joystick_button_size.x * .5f;
                position.y = point.y - parent_offset.y - joystick_button_size.y * .5f;
                
                joystick_button->position = position;
                
                if(m_on_dragging_callback)
                {
                    glm::vec2 container_size= control::size;
                    glm::vec2 center = glm::vec2((container_size.x - joystick_button_size.x) * .5f,
                                                 (container_size.y - joystick_button_size.y) * .5f);
                    delta.x = center.x - position.x;
                    delta.y = center.y - position.y;
                    delta = glm::normalize(delta);
                    f32 angle = atan2(delta.y, delta.x) - atan2(1.f, 0.f);
                    m_on_dragging_callback(shared_from_this(), delta, angle);
                }
            }
        }
        
        void joystick::set_on_dragging_callback(const on_dragging_callback_t& callback)
        {
            m_on_dragging_callback = callback;
        }
        
        void joystick::set_on_end_dragging_callback(const on_end_dragging_callback_t& callback)
        {
            m_on_end_dragging_callback = callback;
        }
        
        void joystick::set_on_double_tap_callback(const on_double_tap_callback_t& callback)
        {
            m_on_double_tap_callback = callback;
        }
    }
}
