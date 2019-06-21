//
//  ces_bound_touch_2d_component.cpp
//  gbCore
//
//  Created by serhii.s on 2/19/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_bound_touch_2d_component.h"

namespace gb
{
    ces_bound_touch_2d_component::ces_bound_touch_2d_component()
    {
        m_mode = e_mode_2d;
    }
    
    ces_bound_touch_2d_component::~ces_bound_touch_2d_component()
    {
        
    }
        
    glm::vec4 ces_bound_touch_2d_component::get_bounds() const
    {
        return m_bounds;
    }
    
    void ces_bound_touch_2d_component::set_bounds(const glm::vec4& bounds)
    {
        m_bounds = bounds;
    }
};

