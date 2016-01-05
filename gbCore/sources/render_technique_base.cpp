//
//  render_technique_base.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "render_technique_base.h"

namespace gb
{
    render_technique_base::render_technique_base(ui32 width, ui32 height, const std::string& name, ui32 index) :
    m_frame_width(width),
    m_frame_height(height),
    m_name(name),
    m_index(index),
    m_clear_color(glm::vec4(0.f, 0.f, 0.f, 1.f))
    {
        
    }
    
    render_technique_base::~render_technique_base(void)
    {
        
    }
    
    ui32 render_technique_base::get_index(void) const
    {
        return m_index;
    }
    
    ui32 render_technique_base::get_frame_width(void) const
    {
        assert(m_frame_width != 0);
        return m_frame_width;
    }
    
    ui32 render_technique_base::get_frame_height(void) const
    {
        assert(m_frame_height != 0);
        return m_frame_height;
    }
    
    std::string render_technique_base::get_name(void) const
    {
        return m_name;
    }
    
    void render_technique_base::set_clear_color(const glm::vec4& color)
    {
        m_clear_color = color;
    }
}