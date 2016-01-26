//
//  content_list_cell.h
//  gbUI
//
//  Created by sergey.sergeev on 1/22/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef content_list_cell_h
#define content_list_cell_h

#include "control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class content_list_cell : public control
        {
        private:
            
            friend class content_list;
            
        protected:
            
            i32 m_data_source_index;
            std::string m_identifier;
            
            void on_touched(const ces_entity_shared_ptr&, const glm::vec2& point,
                            e_input_element input_element, e_input_state input_state);
            
            void set_data_source_index(i32 index);
            
        public:
            
            content_list_cell(const scene_fabricator_shared_ptr& fabricator, i32 data_source_index, const std::string& identifier);
            ~content_list_cell();
            
            void create();
            
            void set_size(const glm::vec2& size);
            
            i32 get_data_source_index() const;
            std::string get_identifier() const;
        };
    };
};

#endif
