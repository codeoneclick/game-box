//
//  drag_controller.h
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/3/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef drag_controller_h
#define drag_controller_h

#include "main_headers.h"
#include "declarations.h"
#include "ed_declarations.h"
#include "input_context.h"

namespace gb
{
    namespace ed
    {
        class drag_controller
        {
        private:
            
        protected:
            
            std::set<std::string> m_callbacks_guids;
            
            glm::vec2 m_previous_dragged_point;
            
            virtual void on_touched(const ces_entity_shared_ptr&, const glm::vec2& point,
                                    e_input_source input_source, e_input_state input_state) = 0;
            virtual void on_dragged(const ces_entity_shared_ptr&, const glm::vec2& point,
                                    e_input_source input_source, e_input_state input_state) = 0;
            
        public:
            
            drag_controller();
            virtual ~drag_controller() = default;
        };
    };
};

#endif
