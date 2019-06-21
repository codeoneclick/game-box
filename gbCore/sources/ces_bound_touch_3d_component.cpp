//
//  ces_bound_touch_3d_component.cpp
//  gbCore
//
//  Created by serhii.s on 2/19/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_bound_touch_3d_component.h"

namespace gb
{
    ces_bound_touch_3d_component::ces_bound_touch_3d_component()
    {
        m_mode = e_mode_3d;
    }
    
    ces_bound_touch_3d_component::~ces_bound_touch_3d_component()
    {
        
    }
    
    glm::vec3 ces_bound_touch_3d_component::get_min_bound() const
    {
        return m_min_bound;
    }
    
    glm::vec3 ces_bound_touch_3d_component::get_max_bound() const
    {
        return m_max_bound;
    }
    
    void ces_bound_touch_3d_component::set_min_bound(const glm::vec3& bound)
    {
        m_min_bound = bound;
    }
    
    void ces_bound_touch_3d_component::set_max_bound(const glm::vec3& bound)
    {
        m_max_bound = bound;
    }
};
