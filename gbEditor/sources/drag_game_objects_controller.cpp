//
//  drag_game_objects_controller.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 1/21/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "drag_game_objects_controller.h"
#include "ces_bound_touch_component.h"
#include "game_object.h"
#include "stroke.h"
#include "ces_transformation_component.h"

namespace gb
{
    namespace ed
    {
        drag_game_objects_controller::drag_game_objects_controller(const stroke_shared_ptr& stroke_object) :
        m_stroke_object(stroke_object),
        m_selected_game_object(nullptr)
        {
    
        }
        
        drag_game_objects_controller::~drag_game_objects_controller()
        {
            for(const auto& entity : m_game_objects)
            {
                gb::ces_bound_touch_component_shared_ptr bound_touch_compoent = std::static_pointer_cast<ces_bound_touch_component>(entity->get_component(e_ces_component_type_bound_touch));
                for (const auto& guid : m_callbacks_guids)
                {
                    bound_touch_compoent->remove_callback(guid);
                }
                bound_touch_compoent->enable(e_input_state_pressed, e_input_source_mouse_left, false);
                bound_touch_compoent->enable(e_input_state_released, e_input_source_mouse_left, false);
                bound_touch_compoent->enable(e_input_state_dragged, e_input_source_mouse_left, false);
            }
            ces_entity_shared_ptr parent = m_stroke_object->parent;
            if(parent)
            {
                parent->remove_child(m_stroke_object);
            }
        }
        
        void drag_game_objects_controller::add_game_object(const game_object_shared_ptr &game_object)
        {
            m_game_objects.insert(game_object);
            
            ces_bound_touch_component_shared_ptr bound_touch_compoent = nullptr;
            if(game_object->is_component_exist(e_ces_component_type_bound_touch))
            {
                bound_touch_compoent = std::static_pointer_cast<ces_bound_touch_component>(game_object->get_component(e_ces_component_type_bound_touch));
            }
            else
            {
                bound_touch_compoent = std::make_shared<ces_bound_touch_component>();
                glm::vec4 bound = game_object->get_bound();
                bound_touch_compoent->set_frame(bound);
                game_object->add_component(bound_touch_compoent);
            }
            
            bound_touch_compoent->enable(e_input_state_dragged, e_input_source_mouse_left, false);
            m_callbacks_guids.insert(bound_touch_compoent->add_callback(e_input_state_dragged, std::bind(&drag_game_objects_controller::on_dragged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
            bound_touch_compoent->enable(e_input_state_pressed, e_input_source_mouse_left, true);
            m_callbacks_guids.insert(bound_touch_compoent->add_callback(e_input_state_pressed, std::bind(&drag_game_objects_controller::on_touched, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
            bound_touch_compoent->enable(e_input_state_released, e_input_source_mouse_left, true);
            m_callbacks_guids.insert(bound_touch_compoent->add_callback(e_input_state_released, std::bind(&drag_game_objects_controller::on_touched, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
        }
        
        void drag_game_objects_controller::on_touched(const ces_entity_shared_ptr& entity, const glm::vec2& point,
                                                      e_input_source input_source, e_input_state input_state)
        {
            drag_controller::on_touched(entity, point, input_source, input_state);
            
            if(input_state == e_input_state_pressed)
            {
                ces_entity_shared_ptr parent = m_stroke_object->parent;
                if(parent)
                {
                    gb::ces_bound_touch_component_shared_ptr bound_touch_compoent =
                    std::static_pointer_cast<ces_bound_touch_component>(parent->get_component(e_ces_component_type_bound_touch));
                    bound_touch_compoent->enable(e_input_state_dragged, e_input_source_mouse_left, false);
                    
                    parent->remove_child(m_stroke_object);
                }
                
                gb::ces_bound_touch_component_shared_ptr bound_touch_compoent = std::static_pointer_cast<ces_bound_touch_component>(entity->get_component(e_ces_component_type_bound_touch));
                bound_touch_compoent->enable(e_input_state_dragged, e_input_source_mouse_left, true);
                
                glm::vec4 bound = bound_touch_compoent->get_frame();
                glm::vec2 size = glm::vec2(fabsf(bound.x) + fabsf(bound.w), fabsf(bound.y) + fabsf(bound.z));
                
                glm::vec2 center = glm::vec2((bound.x + bound.z) * .5f, (bound.y + bound.w) * .5f);
                m_stroke_object->set_size(size);
                m_stroke_object->set_position(center);
                
                entity->add_child(m_stroke_object);
                
                m_selected_game_object = std::static_pointer_cast<game_object>(entity);
            }
        }
        
        void drag_game_objects_controller::on_dragged(const ces_entity_shared_ptr& entity, const glm::vec2& point,
                                                      e_input_source input_source, e_input_state input_state)
        {
            if(m_selected_game_object)
            {
                glm::vec2 delta = point - m_previous_dragged_point;
                glm::vec2 new_position = m_selected_game_object->get_position() + delta;
                m_selected_game_object->set_position(new_position);
            }
            
            drag_controller::on_dragged(entity, point, input_source, input_state);
        }
    }
}