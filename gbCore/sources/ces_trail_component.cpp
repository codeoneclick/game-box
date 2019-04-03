//
//  ces_trail_component.cpp
//  gbCore
//
//  Created by serhii.s on 3/28/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_trail_component.h"
#include "std_extensions.h"

namespace gb
{
    ces_trail_component::ces_trail_component()
    {
        
    }
    
    ces_trail_component::~ces_trail_component()
    {
        
    }
    
    
    void ces_trail_component::set_parameters(ui32 segments, f32 segment_length, f32 segment_width)
    {
        m_segments = segments;
        m_segment_length = segment_length;
        m_segment_width = segment_width;
        
        m_segment_timestamps = std::make_shared<std::vector<f32>>();
        m_segment_timestamps->resize(m_segments, 0.f);
    }
    
    void ces_trail_component::set_start_position(const glm::vec3& position)
    {
        m_old_segment_position = position;
        m_new_segment_position = position;
    }
    
    ui32 ces_trail_component::get_segments_num() const
    {
        return m_segments;
    }
    
    ui32 ces_trail_component::get_used_segments_num() const
    {
        return m_segments_used;
    }
    
    void ces_trail_component::set_used_segments_num(ui32 segments)
    {
        m_segments_used = segments;
    }
    
    f32 ces_trail_component::get_segment_length() const
    {
        return m_segment_length;
    }
    
    f32 ces_trail_component::get_segment_width() const
    {
        return m_segment_width;
    }
    
    void ces_trail_component::set_old_segment_position(const glm::vec3 position)
    {
        m_old_segment_position = position;
    }
    
    glm::vec3 ces_trail_component::get_old_segment_position() const
    {
         return m_old_segment_position;
    }
    
    void ces_trail_component::set_new_segment_position(const glm::vec3 position)
    {
        m_new_segment_position = position;
    }
    
    glm::vec3 ces_trail_component::get_new_segment_position() const
    {
         return m_new_segment_position;
    }
    
    void ces_trail_component::set_old_segment_direction(const glm::vec3& direction)
    {
        m_old_segment_direction = direction;
    }
    
    glm::vec3 ces_trail_component::get_old_segment_direction() const
    {
        return m_old_segment_direction;
    }
    
    void ces_trail_component::set_new_segment_direction(const glm::vec3& direction)
    {
        m_new_segment_direction = direction;
    }
    
    glm::vec3 ces_trail_component::get_new_segment_direction() const
    {
        return m_new_segment_direction;
    }
    
    f32 ces_trail_component::get_min_segment_length() const
    {
        return m_min_segment_length;
    }
    
    f32 ces_trail_component::get_max_visible_time() const
    {
        return m_max_visible_time;
    }
    
    std::shared_ptr<std::vector<f32>> ces_trail_component::get_segment_timestamps() const
    {
        return m_segment_timestamps;
    }
    
    void ces_trail_component::set_enabled(bool value)
    {
        m_is_enabled = value;
        if (m_is_enabled)
        {
            m_emitt_timestamp = std::get_tick_count();
        }
    }
    
    bool ces_trail_component::get_enabled() const
    {
        return m_is_enabled;
    }
    
    bool ces_trail_component::is_expired() const
    {
        bool result = !m_is_enabled;
        if (!m_is_enabled)
        {
            const auto current_time = std::get_tick_count();
            for (i32 i = 0; i < m_segments_used; ++i)
            {
                auto delta = current_time - m_segment_timestamps->data()[i];
                delta = 1.f - glm::clamp(delta / m_max_visible_time, 0.f, 1.f);
                if (delta > 0.f)
                {
                    result = false;
                    break;
                }
            }
        }
        return result;
    }
}
