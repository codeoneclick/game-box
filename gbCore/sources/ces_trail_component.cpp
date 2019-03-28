//
//  ces_trail_component.cpp
//  gbCore
//
//  Created by serhii.s on 3/28/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_trail_component.h"

namespace gb
{
    ces_trail_component::ces_trail_component()
    {
        
    }
    
    ces_trail_component::~ces_trail_component()
    {
        
    }
    
    
    void ces_trail_component::set_parameters(ui32 segments, f32 segment_length, f32 width)
    {
        m_segments = segments;
        m_segment_length = segment_length;
        m_width = width;
    }
    
    void ces_trail_component::set_start_position(const glm::vec3& start_position)
    {
        m_last_segment_position = start_position;
    }
    
    ui32 ces_trail_component::get_segments_num() const
    {
        return m_segments;
    }
    
    ui32 ces_trail_component::get_used_segments_num() const
    {
        return m_segments_used;
    }
    
    f32 ces_trail_component::get_segment_length() const
    {
        return m_segment_length;
    }
    
    f32 ces_trail_component::get_width() const
    {
        return m_width;
    }
}
