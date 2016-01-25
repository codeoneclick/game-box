//
//  content_tab_list.h
//  gbUI
//
//  Created by sergey.sergeev on 1/25/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef content_tab_list_h
#define content_tab_list_h

#include "control.h"

namespace gb
{
    namespace ui
    {
        class content_tab_list_data
        {
        private:
            
        protected:
            
            std::string m_label;
            
        public:
            
            content_tab_list_data(const std::string& label);
            virtual ~content_tab_list_data() = default;
            
            std::string get_label() const;
        };
        
        class content_tab_list : public control
        {
        public:
            
            typedef std::function<content_tab_list_cell_shared_ptr(i32, const content_tab_list_data_shared_ptr&)> t_on_create_cell_callback;
            
        private:
            
        protected:
            
            t_on_create_cell_callback m_on_create_cell_callback;
            glm::vec2 m_separator_offset;
            
            std::vector<content_tab_list_cell_shared_ptr> m_cells;
            std::vector<button_shared_ptr> m_tabs;
            
            void on_tab_pressed(const ces_entity_shared_ptr& entity);
            
        public:
            
            content_tab_list(const scene_fabricator_shared_ptr& fabricator);
            ~content_tab_list();
            
            void create();
            
            void set_size(const glm::vec2& size);
            
            void set_separator_offset(const glm::vec2& separator_offset);
            
            void set_on_create_cell_callback(const t_on_create_cell_callback& callback);
            
            void set_data_source(const std::vector<content_tab_list_data_shared_ptr>& data_source);
        };
    };
};

#endif
