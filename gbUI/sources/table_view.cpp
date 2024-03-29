//
//  table_view.cpp
//  gbUI
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "table_view.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_bound_touch_2d_component.h"
#include "table_view_cell.h"
#include "ces_transformation_2d_component.h"
#include "ces_action_component.h"

namespace gb
{
    namespace ui
    {
        static const i32 k_cache_size_for_unused_cells = 2;
        static const f32 k_scroll_inertia_attenuation = .75f;
        
        table_view::table_view(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_get_cell_callback(nullptr),
        m_get_cell_height_callback(nullptr),
        m_separator_offset(glm::vec2(10.f)),
        m_scroll_inertia(0.f),
        m_drag_events_sum(0.f),
        m_drag_events_count(0)
        {
            ces_entity::add_deferred_component_constructor<ces_bound_touch_2d_component>();
            ces_entity::add_deferred_component_constructor<ces_action_component>();
            
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
                if (bound_touch_component)
                {
                    bound_touch_component->as_2d()->set_bounds(glm::vec4(0.f, 0.f, m_size.x, m_size.y));
                }
                m_elements[control::k_background_element_name]->size = glm::vec2(size.x + m_separator_offset.x * 2.f, size.y);
                
            });
        }
        
        table_view::~table_view()
        {
            
        }
        
        void table_view::setup_components()
        {
            control::setup_components();
            auto bound_touch_component = ces_entity::get_component<ces_bound_touch_component>();
            bound_touch_component->enable(e_input_state_dragged, e_input_source_mouse_left, true);
            bound_touch_component->enable(e_input_state_pressed, e_input_source_mouse_left, true);
            bound_touch_component->enable(e_input_state_released, e_input_source_mouse_left, true);
            bound_touch_component->add_callback(e_input_state_pressed, std::bind(&table_view::on_touched, this,
                                                                                 std::placeholders::_1,
                                                                                 std::placeholders::_2,
                                                                                 std::placeholders::_3,
                                                                                 std::placeholders::_4));
            bound_touch_component->add_callback(e_input_state_released, std::bind(&table_view::on_touched, this,
                                                                                  std::placeholders::_1,
                                                                                  std::placeholders::_2,
                                                                                  std::placeholders::_3,
                                                                                  std::placeholders::_4));
            bound_touch_component->add_callback(e_input_state_dragged, std::bind(&table_view::on_touched, this,
                                                                                 std::placeholders::_1,
                                                                                 std::placeholders::_2,
                                                                                 std::placeholders::_3,
                                                                                 std::placeholders::_4));
            
            auto action_component = ces_entity::get_component<ces_action_component>();
            action_component->set_update_callback(std::bind(&table_view::on_autoscroll, this, std::placeholders::_1, std::placeholders::_2));
        }
        
        void table_view::create()
        {
            gb::sprite_shared_ptr table_view_background =
            control::get_fabricator()->create_sprite("table_view_background.xml");
            m_elements[control::k_background_element_name] = table_view_background;
            table_view::add_child(table_view_background);
            control::set_color(control::k_background_element_name, control::k_dark_gray_color);
            control::create();
        }
        
        void table_view::on_touched(const ces_entity_shared_ptr&,
                                    const glm::vec2& touch_point,
                                    e_input_source input_source,
                                    e_input_state input_state)
        {
            glm::vec2 delta = touch_point - m_previous_dragged_point;
            if(input_state == e_input_state_dragged && m_cells.size() > 0 && delta.y != 0)
            {
                table_view::scroll_content(delta.y);
                if((m_drag_events_sum > 0.f && delta.y < 0.f) ||
                   (m_drag_events_sum < 0.f && delta.y > 0.f))
                {
                    m_drag_events_sum = 0.f;
                    m_drag_events_count = 0.f;
                }
                
                m_drag_events_sum += delta.y;
                m_drag_events_count++;
            }
            else if(input_state == e_input_state_pressed)
            {
                m_drag_events_sum = 0.f;
                m_drag_events_count = 0;
                m_scroll_inertia = 0.f;
            }
            else if(input_state == e_input_state_released)
            {
                m_scroll_inertia = m_drag_events_sum / m_drag_events_count;
            }
            
            m_previous_dragged_point = touch_point;
        }
        
        void table_view::set_separator_offset(const glm::vec2& separator_offset)
        {
            m_separator_offset = separator_offset;
        }
        
        void table_view::set_on_get_cell_callback(const t_get_cell_callback& callback)
        {
            m_get_cell_callback = callback;
        }
        
        void table_view::set_on_update_cell_callback(const t_update_cell_callback& callback)
        {
            m_update_cell_callback = callback;
        }
        
        void table_view::set_on_get_table_cell_height_callback(const t_get_cell_height_callback& callback)
        {
            m_get_cell_height_callback = callback;
        }
        
        void table_view::set_data_source(const std::vector<table_view_cell_data_shared_ptr>& data_source)
        {
            m_data_source = data_source;
        }
        
        const std::vector<table_view_cell_data_shared_ptr>& table_view::get_data_source() const
        {
            return m_data_source;
        }
        
        void table_view::clip_invisible_cells(i32 direction)
        {
            if(m_cells.size() > 1)
            {
                table_view_cell_shared_ptr cell_to_remove = nullptr;
                if(direction == 1)
                {
                    table_view_cell_shared_ptr second_from_front_cell = (*++m_cells.begin());
                    glm::vec2 position = second_from_front_cell->position;
                    i32 index = second_from_front_cell->get_index();
                    f32 cell_height = m_get_cell_height_callback(index);
                    if(position.y + cell_height < 0.f)
                    {
                        cell_to_remove = m_cells.front();
                        m_cells.pop_front();
                    }
                }
                else if(direction == -1)
                {
                    table_view_cell_shared_ptr second_from_back_cell = (*----m_cells.end());
                    glm::vec2 position = second_from_back_cell->position;
                    if(position.y > m_size.y)
                    {
                        cell_to_remove = m_cells.back();
                        m_cells.pop_back();
                    }
                }
                
                if(cell_to_remove)
                {
                    ces_entity::remove_child(cell_to_remove);
                    table_view::clip_invisible_cells(direction);
                    
                    if(m_unused_cells.size() > k_cache_size_for_unused_cells)
                    {
                        m_unused_cells.pop_front();
                    }
                    m_unused_cells.push_back(cell_to_remove);
                    assert(m_unused_cells.size() <= k_cache_size_for_unused_cells);
                }
            }
        }
        
        void table_view::add_visible_cells(i32 direction)
        {
            if(direction == 1)
            {
                glm::vec2 position = m_cells.back()->position;
                i32 index = m_cells.back()->get_index();
                f32 cell_height = m_get_cell_height_callback(index);
                if(position.y + cell_height < m_size.y && index + 1 < m_data_source.size())
                {
                    table_view::fill_cell(index + 1, direction);
                    table_view::add_visible_cells(direction);
                }
            }
            else if(direction == -1)
            {
                glm::vec2 position = m_cells.front()->position;
                i32 index = m_cells.front()->get_index();
                if(position.y > 0.f && index - 1 >= 0)
                {
                    table_view::fill_cell(index - 1, direction);
                    table_view::add_visible_cells(direction);
                }
            }
        }
        
        bool table_view::can_scroll(f32 delta)
        {
            table_view_cell_shared_ptr first_cell = m_cells.front();
            table_view_cell_shared_ptr last_cell = m_cells.back();
            
            glm::vec2 first_cell_position = first_cell->position;
            glm::vec2 last_cell_position = last_cell->position;
            
            return first_cell_position.y + delta <= m_separator_offset.y &&
            last_cell_position.y + delta >= m_size.y - m_separator_offset.y - m_get_cell_height_callback(last_cell->get_index());
        }
        
        bool table_view::scroll_content(f32 delta)
        {
            table_view::clip_invisible_cells(delta < 0.f ? 1 : -1);
            table_view::add_visible_cells(delta < 0.f ? 1 : -1);
            
            if(table_view::can_scroll(delta))
            {
                for(const auto& cell : m_cells)
                {
                    glm::vec2 position = cell->position;
                    position.y += delta;
                    cell->position = position;
                }
                return true;
            }
            return false;
        }
        
        void table_view::on_autoscroll(const gb::ces_entity_shared_ptr& entity, f32 dt)
        {
            if(m_cells.size() != 0)
            {
                if(fabsf(m_scroll_inertia) < 1.f)
                {
                    m_scroll_inertia = 0.f;
                }
                else if(m_scroll_inertia != 0.f)
                {
                    if(table_view::scroll_content(m_scroll_inertia))
                    {
                        m_scroll_inertia *= k_scroll_inertia_attenuation;
                    }
                    else
                    {
                        while(!table_view::can_scroll(m_scroll_inertia) && fabsf(m_scroll_inertia) >= 1.f)
                        {
                            m_scroll_inertia *= k_scroll_inertia_attenuation;
                        }
                        m_scroll_inertia *= .5f;
                    }
                }
            }
        }
        
        void table_view::fill_cell(i32 index, i32 direction)
        {
            f32 offset = 0.f;
            if(direction == 1 && m_cells.size() > 0)
            {
                glm::vec2 position = m_cells.back()->position;
                offset = m_cells.size() != 0 ? position.y + m_separator_offset.y + m_get_cell_height_callback(m_cells.back()->get_index()) : m_separator_offset.y;
            }
            else if(direction == -1 && m_cells.size() > 0)
            {
                glm::vec2 position = m_cells.front()->position;
                offset = m_cells.size() != 0 ? position.y - m_separator_offset.y - m_get_cell_height_callback(index) : -m_separator_offset.y;
            }
            else
            {
                offset = m_separator_offset.y;
            }
            
            table_view_cell_shared_ptr cell = m_get_cell_callback(index, m_data_source[index], shared_from_this());
            cell->position = glm::vec2(m_separator_offset.x, offset);
            cell->set_loading(true);
            table_view::add_child(cell);
            
            if(direction == 1)
            {
                m_cells.push_back(cell);
            }
            else if(direction == -1)
            {
                m_cells.push_front(cell);
            }
        }
        
        void table_view::reload_data()
        {
            for(const auto& cell : m_cells)
            {
                ces_entity::remove_child(cell);
            }
            m_cells.clear();
            m_unused_cells.clear();
            
            f32 visible_content_height = m_size.y;
            visible_content_height -= m_separator_offset.y;
            i32 max_visible_index = 0;
            
            for(i32 i = 0; i < m_data_source.size(); ++i)
            {
                f32 cell_height = m_get_cell_height_callback(i);
                visible_content_height -= cell_height;
                if(visible_content_height > 0.f)
                {
                    max_visible_index++;
                }
                visible_content_height -= m_separator_offset.y;
            }
            
            max_visible_index = std::min(max_visible_index + 2, static_cast<i32>(m_data_source.size()));
            
            for(i32 i = 0; i < max_visible_index; ++i)
            {
                table_view::fill_cell(i, 1);
            }
            table_view::scroll_content(m_separator_offset.y - 1.f);
        }
        
        void table_view::update_data()
        {
            if (m_update_cell_callback)
            {
                for (const auto& cell : m_cells)
                {
                    m_update_cell_callback(cell->get_index(), m_data_source[cell->get_index()], shared_from_this(), cell);
                }
            }
        }
        
        table_view_cell_shared_ptr table_view::reuse_cell(const std::string& identifier, i32 index)
        {
            table_view_cell_shared_ptr cell = nullptr;
            const auto& iterator = std::find_if(m_unused_cells.begin(), m_unused_cells.end(), [identifier](table_view_cell_shared_ptr cell)->bool {
                return cell->get_identifier() == identifier;
            });
            if(iterator != m_unused_cells.end())
            {
                cell = (*iterator);
                cell->set_index(index);
                m_unused_cells.erase(iterator);
            }
            return cell;
        }
        
        std::shared_ptr<scene_fabricator> table_view::get_fabricator() const
        {
            return m_fabricator.lock();
        }
    }
}
