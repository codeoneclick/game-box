//
//  tree_view.hpp
//  gbUI
//
//  Created by Serhii Serhiiv on 2/17/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef tree_view_h
#define tree_view_h

#include "control.h"

namespace gb
{
    namespace ui
    {
        class tree_view : public control
        {
        private:
            
            std::map<std::string, tree_view_cell_shared_ptr> m_named_cells;
            
        protected:
            
            tree_view_cell_data_shared_ptr m_data_source;
            
            void add_cells_recursively(const tree_view_cell_data_shared_ptr& data, f32 offset_x, f32* offset_y);
            void update_cells_recursively(const tree_view_cell_data_shared_ptr& data, f32 offset_x, f32* offset_y);
            
            void on_expand(const tree_view_cell_data_shared_ptr& data, const ces_entity_shared_ptr& entity);
            void expand_recursively(const tree_view_cell_data_shared_ptr& data, bool is_expand);
            
        public:
            
            tree_view(const scene_fabricator_shared_ptr& fabricator);
            ~tree_view();
            
            void create();
            
            void set_data_source(const tree_view_cell_data_shared_ptr& data_source);
            void reload_data();
            void clear();
            
            std::property_rw<glm::vec2> size;
        };
    };
};

#endif
