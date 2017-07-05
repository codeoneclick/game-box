//
//  tree_view.cpp
//  gbUI
//
//  Created by Serhii Serhiiv on 2/17/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "tree_view.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_bound_touch_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_action_component.h"
#include "ces_material_component.h"
#include "tree_view_cell.h"

namespace gb
{
    namespace ui
    {
        tree_view::tree_view(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator)
        {
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                m_elements["tree_view_background"]->size = size;
                
            });
        }
        
        tree_view::~tree_view()
        {
            
        }
        
        tree_view_shared_ptr tree_view::construct(const scene_fabricator_shared_ptr& fabricator)
        {
            auto entity = std::make_shared<tree_view>(fabricator);
            return entity;
        }
        
        void tree_view::create()
        {
            gb::sprite_shared_ptr tree_view_background =
            control::get_fabricator()->create_sprite("tree_view_background.xml");
            m_elements["tree_view_background"] = tree_view_background;
            tree_view::add_child(tree_view_background);
            
            control::create();
        }
        
        void tree_view::set_data_source(const tree_view_cell_data_shared_ptr& data_source)
        {
            m_data_source = data_source;
        }
        
        void tree_view::add_cells_recursively(const tree_view_cell_data_shared_ptr& data, f32 offset_x, f32* offset_y)
        {
            std::list<tree_view_cell_data_shared_ptr> children_data = data->children;
            tree_view_cell_shared_ptr cell = std::make_shared<gb::ui::tree_view_cell>(tree_view::get_fabricator(), data);
            cell->create();
            cell->size = glm::vec2(128.f, 24.f);
            cell->position = glm::vec2(offset_x, (*offset_y) += 28.f);
            cell->text = data->description;
            cell->has_children = children_data.size() != 0;
            cell->on_expand_callback = std::bind(&tree_view::on_expand, this, std::placeholders::_1, std::placeholders::_2);
            tree_view::add_child(cell);
            
            m_named_cells.insert(std::make_pair(data->description, cell));
            
            for(const auto& child_data : children_data)
            {
                tree_view::add_cells_recursively(child_data, offset_x + 24.f, offset_y);
            }
        }
        
        void tree_view::update_cells_recursively(const tree_view_cell_data_shared_ptr& data, f32 offset_x, f32* offset_y)
        {
            const auto& cell = m_named_cells.find(data->description);
            if(cell != m_named_cells.end() && cell->second->is_expanded)
            {
                std::list<tree_view_cell_data_shared_ptr> children_data = data->children;
                
                cell->second->position = glm::vec2(offset_x, (*offset_y) += 28.f);
                cell->second->has_children = children_data.size() != 0;
                
                for(const auto& child_data : children_data)
                {
                    tree_view::update_cells_recursively(child_data, offset_x + 24.f, offset_y);
                }
            }
            else if(cell != m_named_cells.end())
            {
                std::list<tree_view_cell_data_shared_ptr> children_data = data->children;
                
                cell->second->position = glm::vec2(offset_x, (*offset_y) += 28.f);
                cell->second->has_children = children_data.size() != 0;
            }
        }
        
        void tree_view::reload_data()
        {
            tree_view::clear();
            
            f32 offset_x = 12.f;
            f32 offset_y = 0.f;
            tree_view::add_cells_recursively(m_data_source, offset_x, &offset_y);
        }
        
        void tree_view::clear()
        {
            for(const auto& cell : m_named_cells)
            {
                tree_view::remove_child(cell.second);
            }
            m_named_cells.clear();
        }
        
        void tree_view::on_expand(const tree_view_cell_data_shared_ptr& data, const ces_entity_shared_ptr& entity)
        {
            tree_view_cell_shared_ptr cell = std::static_pointer_cast<tree_view_cell>(entity);
            
            std::list<tree_view_cell_data_shared_ptr> children_data = data->children;
            for(const auto& child_data : children_data)
            {
                tree_view::expand_recursively(child_data, !cell->is_expanded);
            }
            
            cell->is_expanded = !cell->is_expanded;
            
            f32 offset_x = 12.f;
            f32 offset_y = 0.f;
            tree_view::update_cells_recursively(m_data_source, offset_x, &offset_y);
        }
        
        void tree_view::expand_recursively(const tree_view_cell_data_shared_ptr& data, bool is_expand)
        {
            const auto& cell = m_named_cells.find(data->description);
            if(cell != m_named_cells.end())
            {
                cell->second->visible = is_expand;
            }
            
            std::list<tree_view_cell_data_shared_ptr> children_data = data->children;
            for(const auto& child_data : children_data)
            {
                tree_view::expand_recursively(child_data, is_expand);
            }
        }
    }
}
