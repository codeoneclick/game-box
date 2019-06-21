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
#include "ces_bound_touch_2d_component.h"
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
        gb::ui::interaction_control(fabricator)
        {
            ces_entity::add_deferred_component_constructor<ces_bound_touch_2d_component>();
            
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                interaction_control::on_touch_size_changed(m_size);
                
                std::static_pointer_cast<gb::sprite>(m_elements["joystick_background"])->size = size;
                std::static_pointer_cast<gb::sprite>(m_elements["joystick_button"])->size = size * .5f;
                
                control::set_element_horizontal_aligment(m_elements["joystick_button"], e_element_horizontal_aligment_center);
                control::set_element_vertical_aligment(m_elements["joystick_button"], e_element_vertical_aligment_center);
            });
        }
        
        void joystick::setup_components()
        {
            interaction_control::setup_components();
        }
        
        void joystick::create()
        {
            gb::sprite_shared_ptr joystick_background = control::get_fabricator()->create_sprite("joystick_background.xml");
            m_elements["joystick_background"] = joystick_background;
            joystick::add_child(joystick_background);
            
            gb::sprite_shared_ptr joystick_button = control::get_fabricator()->create_sprite("joystick_button.xml");
            m_elements["joystick_button"] = joystick_button;
            joystick::add_child(joystick_button);
            
            interaction_control::create();
        }
        
        void joystick::on_touched(const ces_entity_shared_ptr& entity,
                                  const glm::vec2& touch_point,
                                  e_input_source input_source,
                                  e_input_state input_state)
        {
            interaction_control::on_touched(entity, touch_point, input_source, input_state);
            control::set_element_horizontal_aligment(m_elements["joystick_button"], e_element_horizontal_aligment_center);
            control::set_element_vertical_aligment(m_elements["joystick_button"], e_element_vertical_aligment_center);
        }
        
        void joystick::on_dragging(const ces_entity_shared_ptr&,
                                   const glm::vec2& touch_point,
                                   e_input_source input_source,
                                   e_input_state input_state)
        {
            gb::sprite_shared_ptr joystick_button = std::static_pointer_cast<gb::sprite>(m_elements["joystick_button"]);
            gb::sprite_shared_ptr joystick_background = std::static_pointer_cast<gb::sprite>(m_elements["joystick_background"]);
            glm::vec2 joystick_button_size = joystick_button->size;
            m_is_dragging = true;
            
            glm::vec2 parent_offset = glm::vec2(0.f);
            
            ces_entity_shared_ptr parent = joystick_button->parent;
            while (parent)
            {
                auto transformation_component = parent->get_component<ces_transformation_2d_component>();
                glm::vec2 parent_position = transformation_component->get_position();
                parent_offset.x += parent_position.x;
                parent_offset.y += parent_position.y;
                parent = parent->parent;
            }
            
            glm::vec2 position = joystick_button->position;
            glm::vec2 delta = position;
                
            position.x = touch_point.x - parent_offset.x - joystick_button_size.x * .5f;
            position.y = touch_point.y - parent_offset.y - joystick_button_size.y * .5f;
                
            joystick_button->position = position;
                
            if(m_on_dragging_callback)
            {
                glm::vec2 container_size = control::size;
                glm::vec2 center = glm::vec2((container_size.x - joystick_button_size.x) * .5f,
                                             (container_size.y - joystick_button_size.y) * .5f);
                delta.x = center.x - position.x;
                delta.y = center.y - position.y;
                delta.x = std::fabsf(delta.x) > m_threshold.x ? delta.x : 0.f;
                delta.y = std::fabsf(delta.y) > m_threshold.y ? delta.y : 0.f;
                delta = glm::normalize(delta);
                m_on_dragging_callback(shared_from_this(), delta);
            }
        }
    
        bool joystick::is_drag_started_callback_exist() const
        {
            return m_on_drag_started_callback != nullptr;
        }
        
        bool joystick::is_dragging_callback_exist() const
        {
            return m_on_dragging_callback != nullptr;
        }
        
        bool joystick::is_drag_ended_callback_exist() const
        {
            return m_on_drag_ended_callback != nullptr;
        }
        
        void joystick::set_threshold(const glm::vec2& threshold)
        {
            m_threshold = threshold;
        }
    }
}
