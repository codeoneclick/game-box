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
        class content_list : public control
        {
        private:
            
        protected:
            
            void on_touched(const ces_entity_shared_ptr&, const glm::vec2& point, e_input_element input_element, e_input_state input_state);
            
        public:
            
            content_list(const scene_fabricator_shared_ptr& fabricator);
            ~content_list();
            
            void create();
            
            void set_size(const glm::vec2& size);
            glm::vec2 get_size() const;
            
            void set_data_source();
        };
    };
};

#endif
