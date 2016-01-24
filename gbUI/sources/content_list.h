//
//  content_list.h
//  gbUI
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef content_list_h
#define content_list_h

#include "control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class content_list_data
        {
        private:
            
        protected:
            
        public:
            
            content_list_data() = default;
            virtual ~content_list_data() = default;
        };
        
        class content_list : public control
        {
        public:
            
            typedef std::function<content_list_cell_shared_ptr(i32, const content_list_data_shared_ptr&)> t_create_cell_callback;
            
        private:
            
        protected:
            
            t_create_cell_callback m_create_cell_callback;
            
            glm::vec2 m_separator_offset;
            
            void on_touched(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_element input_element, e_input_state input_state);
            
        public:
            
            content_list(const scene_fabricator_shared_ptr& fabricator);
            ~content_list();
            
            void create();
            
            void set_size(const glm::vec2& size);
            
            void set_separator_offset(const glm::vec2& separator_offset);
            
            void set_create_cell_callback(const t_create_cell_callback& callback);
            
            void set_data_source(const std::vector<content_list_data_shared_ptr>& data_source);
        };
    };
};

#endif
