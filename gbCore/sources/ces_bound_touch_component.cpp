//
//  ces_bound_touch_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_bound_touch_component.h"

namespace gb
{
    ces_bound_touch_component::ces_bound_touch_component() :
    m_frame(0.f)
    {
        m_type = e_ces_component_type_bound_touch;
    }
    
    ces_bound_touch_component::~ces_bound_touch_component()
    {
        
    }
    
    void ces_bound_touch_component::set_frame(const glm::vec4& frame)
    {
        m_frame = frame;
    }
    
    glm::vec4 ces_bound_touch_component::get_frame() const
    {
        return m_frame;
    }
};