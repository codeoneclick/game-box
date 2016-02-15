//
//  drag_brush_controller.cpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/2/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "drag_brush_controller.h"
#include "canvas.h"
#include "brush.h"
#include "ces_material_component.h"
#include "ces_bound_touch_component.h"
#include "ces_scene_component.h"
#include "camera.h"

namespace gb
{
    namespace ed
    {
        drag_brush_controller::drag_brush_controller(const canvas_shared_ptr& canvas, const brush_shared_ptr& brush) :
        m_canvas(canvas),
        m_brush(brush),
        m_grid(nullptr)
        {
            drag_brush_controller::set_active_brush(0, 0);
        }
        
        drag_brush_controller::~drag_brush_controller()
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
                bound_touch_compoent->enable(e_input_state_moved, e_input_source_none, false);
            }
            
            ces_entity_shared_ptr parent = m_brush->parent;
            
            if(parent)
            {
                parent->remove_child(m_brush);
            }
        }
        
        void drag_brush_controller::set_grid(const game_object_shared_ptr& grid)
        {
            m_grid = grid;
            
            ces_bound_touch_component_shared_ptr bound_touch_compoent = std::make_shared<ces_bound_touch_component>();
            bound_touch_compoent->enable(e_input_state_dragged, e_input_source_mouse_left, false);
            m_callbacks_guids.insert(bound_touch_compoent->add_callback(e_input_state_dragged, std::bind(&drag_brush_controller::on_dragged, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
            bound_touch_compoent->enable(e_input_state_pressed, e_input_source_mouse_left, true);
            m_callbacks_guids.insert(bound_touch_compoent->add_callback(e_input_state_pressed, std::bind(&drag_brush_controller::on_touched, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
            bound_touch_compoent->enable(e_input_state_released, e_input_source_mouse_left, true);
            m_callbacks_guids.insert(bound_touch_compoent->add_callback(e_input_state_released, std::bind(&drag_brush_controller::on_touched, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
            bound_touch_compoent->enable(e_input_state_moved, e_input_source_none, true);
            m_callbacks_guids.insert(bound_touch_compoent->add_callback(e_input_state_moved, std::bind(&drag_brush_controller::on_moved, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3, std::placeholders::_4)));
            m_grid->add_component(bound_touch_compoent);
            bound_touch_compoent->set_frame(m_grid->bound);
            
            m_grid->add_child(m_brush);
        }
        
        void drag_brush_controller::set_active_brush(i32 layer, i32 sampler)
        {
            texture_shared_ptr brush_texture = m_brush->get_active_texture();
            ces_material_component_shared_ptr material_component = std::static_pointer_cast<ces_material_component>(m_brush->get_component(e_ces_component_type_material));
            material_component->set_texture(brush_texture, e_shader_sampler_01);
        }
        
        void drag_brush_controller::on_touched(const ces_entity_shared_ptr& entity, const glm::vec2& point, e_input_source input_source, e_input_state input_state)
        {
            drag_controller::on_touched(entity, point, input_source, input_state);
            
            ces_scene_component_shared_ptr scene_component = std::static_pointer_cast<ces_scene_component>(m_grid->get_component(e_ces_component_type_scene));
            
            glm::vec2 position = glm::vec2(point.x,
                                           point.y) - scene_component->get_camera()->get_position();
            
            m_brush->set_position(position);
            m_brush->visible = input_state == e_input_state_pressed;
            m_canvas->draw(position, m_brush->get_active_texture());
        }
        
        void drag_brush_controller::on_dragged(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state)
        {
            ces_scene_component_shared_ptr scene_component = std::static_pointer_cast<ces_scene_component>(m_grid->get_component(e_ces_component_type_scene));
            
            m_brush->set_position(glm::vec2(point.x - m_brush->get_radius() * .5f,
                                            point.y - m_brush->get_radius() * .5f) - scene_component->get_camera()->get_position());
        }
        
        void drag_brush_controller::on_moved(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_source input_source, e_input_state input_state)
        {
            
        }
    }
}