//
//  content_tab_list_cell.h
//  gbUI
//
//  Created by sergey.sergeev on 1/25/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef content_tab_list_cell_h
#define content_tab_list_cell_h

#include "control.h"
#include "input_context.h"

namespace gb
{
    namespace ui
    {
        class content_tab_list_cell : public control
        {
        private:
            
        protected:
            
            void on_touched(const ces_entity_shared_ptr&, const glm::vec2& point,
                            e_input_source input_source, e_input_state input_state);
            
        public:
            
            content_tab_list_cell(const scene_fabricator_shared_ptr& fabricator);
            ~content_tab_list_cell();
            
            static content_tab_list_cell_shared_ptr construct(const scene_fabricator_shared_ptr& fabricator);
            
            void create();
        };
    };
};


#endif 
