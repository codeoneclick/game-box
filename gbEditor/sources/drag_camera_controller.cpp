//
//  drag_camera_controller.cpp
//  gbDemo
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "drag_camera_controller.h"
#include "camera.h"
#include "grid.h"
#include "ces_bound_touch_component.h"

namespace gb
{
    namespace ed
    {
        drag_camera_controller::drag_camera_controller(const gb::camera_shared_ptr& camera) :
        m_camera(camera)
        {
            
        }
        
        drag_camera_controller::~drag_camera_controller()
        {
            if(m_grid)
            {
                gb::ces_bound_touch_component_shared_ptr bound_touch_compoent = std::static_pointer_cast<ces_bound_touch_component>(m_grid->get_component(e_ces_component_type_bound_touch));
                for(const auto& guid : m_callbacks_guids)
                {
                    bound_touch_compoent->remove_callback(guid);
                }
                bound_touch_compoent->enable(e_input_state_pressed, e_input_source_mouse_left, false);
                bound_touch_compoent->enable(e_input_state_released, e_input_source_mouse_left, false);
                bound_touch_compoent->enable(e_input_state_dragged, e_input_source_mouse_left, false);
            }
        }
        
        void drag_camera_controller::set_grid(const gb::game_object_shared_ptr& grid)
        {
            m_grid = grid;
            
            if(m_grid)
            {
                ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<ces_bound_touch_component>();
                bound_touch_compoent->enable(e_input_state_dragged, e_input_source_mouse_left, false);
                m_callbacks_guids.insert(bound_touch_compoent->add_callback(e_input_state_dragged, std::bind(&drag_camera_controller::on_dragged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
                bound_touch_compoent->enable(e_input_state_pressed, e_input_source_mouse_left, true);
                m_callbacks_guids.insert(bound_touch_compoent->add_callback(e_input_state_pressed, std::bind(&drag_camera_controller::on_touched, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
                bound_touch_compoent->enable(e_input_state_released, e_input_source_mouse_left, true);
                m_callbacks_guids.insert(bound_touch_compoent->add_callback(e_input_state_released, std::bind(&drag_camera_controller::on_touched, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
                m_grid->add_component(bound_touch_compoent);
                bound_touch_compoent->set_frame(m_grid->bound);
            }
        }
        
        void drag_camera_controller::on_touched(const ces_entity_shared_ptr& entity, const glm::vec2& point, e_input_source input_source, e_input_state input_state)
        {
            drag_controller::on_touched(entity, point, input_source, input_state);
            
            if(input_state == e_input_state_pressed)
            {
                gb::ces_bound_touch_component_shared_ptr bound_touch_compoent = std::static_pointer_cast<ces_bound_touch_component>(m_grid->get_component(e_ces_component_type_bound_touch));
                bound_touch_compoent->enable(e_input_state_dragged, e_input_source_mouse_left, true);
            }
            else if(input_state == e_input_state_released)
            {
                gb::ces_bound_touch_component_shared_ptr bound_touch_compoent = std::static_pointer_cast<ces_bound_touch_component>(m_grid->get_component(e_ces_component_type_bound_touch));
                bound_touch_compoent->enable(e_input_state_dragged, e_input_source_mouse_left, false);
            }
        }
        
        void drag_camera_controller::on_dragged(const ces_entity_shared_ptr& entity, const glm::vec2& point, e_input_source input_source, e_input_state input_state)
        {
            glm::vec2 delta = point - m_previous_dragged_point;
            glm::vec2 new_camera_position = m_camera->get_position() + delta;
            glm::vec4 grid_bound = m_grid->bound;
            glm::ivec2 screen_size = m_camera->screen_size;
            new_camera_position = glm::clamp(new_camera_position, -glm::vec2(grid_bound.z - screen_size.x,
                                                                             grid_bound.w - screen_size.y),
                                             glm::vec2(grid_bound.x, grid_bound.y));
            m_camera->set_position(new_camera_position);
            
            drag_controller::on_dragged(entity, point, input_source, input_state);
        }
    }
}