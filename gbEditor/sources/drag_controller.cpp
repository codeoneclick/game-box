//
//  drag_controller.cpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/3/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "drag_controller.h"

namespace gb
{
    namespace ed
    {
        drag_controller::drag_controller() :
        m_previous_dragged_point(glm::vec2(0.f))
        {
            
        }
        
        void drag_controller::on_touched(const ces_entity_shared_ptr&, const glm::vec2& point,
                                         e_input_source input_source, e_input_state input_state)
        {
            m_previous_dragged_point = point;
        }
        
        void drag_controller::on_dragged(const ces_entity_shared_ptr&, const glm::vec2& point,
                                         e_input_source input_source, e_input_state input_state)
        {
            m_previous_dragged_point = point;
        }
    }
}