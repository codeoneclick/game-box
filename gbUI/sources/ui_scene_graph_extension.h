//
//  scene_graph_extension.hpp
//  gbUI
//
//  Created by Serhii Serhiiv on 2/17/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ui_scene_graph_extension_h
#define ui_scene_graph_extension_h

#include "tree_view_cell.h"

namespace gb
{
    namespace ui
    {
        class tree_view_cell_scene_graph_data : public tree_view_cell_data
        {
        private:
            
        protected:
            
            ces_entity_shared_ptr m_entity;
            
        public:
            
            tree_view_cell_scene_graph_data(const std::string& description, const ces_entity_shared_ptr& entity);
            ~tree_view_cell_scene_graph_data();
        };
        
        class scene_graph_extension
        {
        private:
            
        protected:
            
            static void disassembly_scene_recursively(const ces_entity_shared_ptr& entity,
                                                      const tree_view_cell_scene_graph_data_shared_ptr& data_source);
            
        public:
            
            scene_graph_extension() = default;
            ~scene_graph_extension() = default;
            
            static tree_view_cell_scene_graph_data_shared_ptr convert_scene_to_data_source(const ces_entity_shared_ptr& root);
        };
    };
};

#endif
