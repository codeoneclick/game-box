//
//  scene_graph_extension.cpp
//  gbUI
//
//  Created by Serhii Serhiiv on 2/17/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ui_scene_graph_extension.h"
#include "scene_graph.h"
#include "ces_transformation_2d_component.h"

namespace gb
{
    namespace ui
    {
        tree_view_cell_scene_graph_data::tree_view_cell_scene_graph_data(const std::string& description, const ces_entity_shared_ptr& entity) : gb::ui::tree_view_cell_data(description),
        m_entity(entity)
        {
            
        }
        
        tree_view_cell_scene_graph_data::~tree_view_cell_scene_graph_data()
        {
            
        }
        
        void scene_graph_extension::disassembly_scene_recursively(const ces_entity_shared_ptr& entity,
                                                                  const tree_view_cell_scene_graph_data_shared_ptr& data_source)
        {
            std::string tag = entity->tag;
            f32 z_order = 0.f;
            ces_transformation_component_shared_ptr transformation_component = entity->get_component<ces_transformation_2d_component>();
            if(transformation_component)
            {
                z_order = transformation_component->get_z_order();
            }
            
            std::ostringstream stream;
            stream<<tag;
            stream<<"  z_order: ";
            stream<<z_order;
            
            tree_view_cell_scene_graph_data_shared_ptr child_data_source = std::make_shared<tree_view_cell_scene_graph_data>( stream.str(), entity);
            data_source->add_child(child_data_source);
            
            std::list<ces_entity_shared_ptr> children = entity->children;
            for(const auto& child : children)
            {
                scene_graph_extension::disassembly_scene_recursively(child, child_data_source);
            }
        }
        
        tree_view_cell_scene_graph_data_shared_ptr scene_graph_extension::convert_scene_to_data_source(const ces_entity_shared_ptr& root)
        {
            tree_view_cell_scene_graph_data_shared_ptr data_source = std::make_shared<tree_view_cell_scene_graph_data>(root->tag, root);
            std::list<ces_entity_shared_ptr> children = root->children;
            for(const auto& child : children)
            {
                scene_graph_extension::disassembly_scene_recursively(child, data_source);
            }
            return data_source;
        }
    };
};
