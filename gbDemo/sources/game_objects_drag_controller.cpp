//
//  game_objects_drag_controller.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 1/21/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "game_objects_drag_controller.h"
#include "ces_bound_touch_component.h"
#include "game_object.h"
#include "stroke.h"
#include "ces_transformation_component.h"

namespace cs
{
    game_objects_drag_controller::game_objects_drag_controller(const gb::ed::stroke_shared_ptr& stroke_object) :
    m_stroke_object(stroke_object),
    m_selected_game_object(nullptr)
    {

    }
    
    game_objects_drag_controller::~game_objects_drag_controller()
    {
        
    }
    
    void game_objects_drag_controller::add_game_object(const gb::game_object_shared_ptr &game_object)
    {
        m_game_objects.insert(game_object);
        
        gb::ces_bound_touch_component_shared_ptr bound_touch_compoent = nullptr;
        if(game_object->is_component_exist(gb::e_ces_component_type_bound_touch))
        {
            bound_touch_compoent = std::static_pointer_cast<gb::ces_bound_touch_component>(game_object->get_component(gb::e_ces_component_type_bound_touch));
        }
        else
        {
            bound_touch_compoent = std::make_shared<gb::ces_bound_touch_component>();
            glm::vec4 bound = game_object->get_bound();
            bound_touch_compoent->set_frame(bound);
            game_object->add_component(bound_touch_compoent);
        }
        
        bound_touch_compoent->enable(gb::e_input_state_dragged, false);
        bound_touch_compoent->set_callback(gb::e_input_state_dragged, std::bind(&game_objects_drag_controller::on_dragged, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        bound_touch_compoent->enable(gb::e_input_state_pressed, true);
        bound_touch_compoent->set_callback(gb::e_input_state_pressed, std::bind(&game_objects_drag_controller::on_touched, this, std::placeholders::_1,
                                                                                std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
        bound_touch_compoent->enable(gb::e_input_state_released, true);
        bound_touch_compoent->set_callback(gb::e_input_state_released, std::bind(&game_objects_drag_controller::on_touched, this, std::placeholders::_1,
                                                                                 std::placeholders::_2, std::placeholders::_3, std::placeholders::_4));
    }
    
    void game_objects_drag_controller::on_touched(const gb::ces_entity_shared_ptr& entity, const glm::vec2& point,
                                                  gb::e_input_element input_element, gb::e_input_state input_state)
    {
        m_previous_dragged_point = point;

        if(input_state == gb::e_input_state_pressed)
        {
            if(m_stroke_object->get_parent())
            {
                gb::ces_bound_touch_component_shared_ptr bound_touch_compoent =
                std::static_pointer_cast<gb::ces_bound_touch_component>(m_stroke_object->get_parent()->get_component(gb::e_ces_component_type_bound_touch));
                bound_touch_compoent->enable(gb::e_input_state_dragged, false);
                
                m_stroke_object->get_parent()->remove_child(m_stroke_object);
            }
            
            gb::ces_bound_touch_component_shared_ptr bound_touch_compoent = std::static_pointer_cast<gb::ces_bound_touch_component>(entity->get_component(gb::e_ces_component_type_bound_touch));
            bound_touch_compoent->enable(gb::e_input_state_dragged, true);
            
            glm::vec4 bound = bound_touch_compoent->get_frame();
            glm::vec2 size = glm::vec2(fabsf(bound.x) + fabsf(bound.w), fabsf(bound.y) + fabsf(bound.z));
            
            glm::vec2 center = glm::vec2((bound.x + bound.z) * .5f, (bound.y + bound.w) * .5f);
            m_stroke_object->set_size(size);
            m_stroke_object->set_position(center);
            
            entity->add_child(m_stroke_object);
            
            m_selected_game_object = std::static_pointer_cast<gb::game_object>(entity);
        }
    }
    
    void game_objects_drag_controller::on_dragged(const gb::ces_entity_shared_ptr&, const glm::vec2& point,
                                                  gb::e_input_element input_element, gb::e_input_state input_state)
    {
        if(m_selected_game_object)
        {
            glm::vec2 delta = point - m_previous_dragged_point;
            glm::vec2 new_position = m_selected_game_object->get_position() + delta;
            m_selected_game_object->set_position(new_position);
        }
        m_previous_dragged_point = point;
    }
}