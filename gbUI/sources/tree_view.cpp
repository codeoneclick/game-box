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
#include "ces_transformation_component.h"
#include "ces_action_component.h"
#include "ces_material_component.h"
#include "tree_view_cell.h"

namespace gb
{
    namespace ui
    {
        static const std::string k_color_state_uniform = "u_color";
        
        tree_view::tree_view(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator)
        {
            size.setter([=](const glm::vec2& size) {
                control::set_size(size);
                std::static_pointer_cast<gb::sprite>(m_elements["tree_view_background"])->size = size;
            });
            size.getter([=]() {
                return control::get_size();
            });
        }
        
        tree_view::~tree_view()
        {
            
        }
        
        void tree_view::create()
        {
            gb::sprite_shared_ptr tree_view_background =
            control::get_fabricator()->create_sprite("tree_view_background.xml");
            m_elements["tree_view_background"] = tree_view_background;
            ces_entity::add_child(tree_view_background);
            
            ces_material_component* material_component = unsafe_get_material_component(tree_view_background);
            material_component->set_custom_shader_uniform(control::k_dark_gray_color, k_color_state_uniform);
            
            control::create();
        }
        
        void tree_view::set_data_source(const tree_view_cell_data_shared_ptr& data_source)
        {
            m_data_source = data_source;
        }
        
        void tree_view::add_cells_recursively(const tree_view_cell_data_shared_ptr& data, f32 offset_x, f32* offset_y)
        {
            tree_view_cell_shared_ptr cell = std::make_shared<gb::ui::tree_view_cell>(tree_view::get_fabricator(), data);
            cell->create();
            cell->size = glm::vec2(128.f, 24.f);
            cell->position = glm::vec2(offset_x, (*offset_y) += 28.f);
            cell->text = data->description;
            ces_entity::add_child(cell);
            
            std::list<tree_view_cell_data_shared_ptr> children_data =  data->children;
            for(const auto& child_data : children_data)
            {
                tree_view::add_cells_recursively(child_data, offset_x + 24.f, offset_y);
            }
        }
        
        void tree_view::reload_data()
        {
            f32 offset_x = 12.f;
            f32 offset_y = 0.f;
            tree_view::add_cells_recursively(m_data_source, offset_x, &offset_y);
        }
    }
}