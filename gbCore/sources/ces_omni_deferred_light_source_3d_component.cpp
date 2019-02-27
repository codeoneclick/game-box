//
//  ces_omni_deferred_light_source_3d_component.cpp
//  gbCore
//
//  Created by serhii.s on 2/27/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_omni_deferred_light_source_3d_component.h"

namespace gb
{
    void ces_omni_deferred_light_source_3d_component::set_radius(f32 radius)
    {
        m_radius = radius;
    }
    
    void ces_omni_deferred_light_source_3d_component::set_color(const glm::vec4& color)
    {
        m_color = color;
    }
    
    f32 ces_omni_deferred_light_source_3d_component::get_radius() const
    {
        return m_radius;
    }
    
    glm::vec4 ces_omni_deferred_light_source_3d_component::get_color() const
    {
        return m_color;
    }
};
