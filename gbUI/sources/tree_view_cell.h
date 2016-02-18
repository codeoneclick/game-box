//
//  tree_view_cell.hpp
//  gbUI
//
//  Created by Serhii Serhiiv on 2/17/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef tree_view_cell_h
#define tree_view_cell_h

#include "control.h"

namespace gb
{
    namespace ui
    {
        class tree_view_cell_data : public std::enable_shared_from_this<tree_view_cell_data>
        {
        private:
            
        protected:
            
            std::weak_ptr<tree_view_cell_data> m_parent;
            std::set<tree_view_cell_data_shared_ptr> m_children;
            std::list<tree_view_cell_data_shared_ptr> m_ordered_children;
            std::string m_description;
            
        public:
            
            tree_view_cell_data(const std::string& description);
            virtual ~tree_view_cell_data() = default;
            
            void add_child(const tree_view_cell_data_shared_ptr& child);
            void remove_child(const tree_view_cell_data_shared_ptr& child);
            
            std::property_ro<tree_view_cell_data_shared_ptr> parent;
            std::property_ro<std::list<tree_view_cell_data_shared_ptr>> children;
            std::property_ro<std::string> description;
        };
        
        class tree_view_cell : public control
        {
        private:
            
            e_element_horizontal_aligment m_text_horizontal_aligment;
            e_element_vertical_aligment m_text_vertical_aligment;
            
        protected:
            
            tree_view_cell_data_shared_ptr m_data;
            
            void on_text_mesh_updated();
            void on_text_updated();
            
        public:
            
            tree_view_cell(const scene_fabricator_shared_ptr& fabricator, const tree_view_cell_data_shared_ptr& data);
            ~tree_view_cell();
            
            void create();
            
            std::property_rw<glm::vec2> size;
            std::property_rw<std::string> text;
        };
    };
};

#endif
