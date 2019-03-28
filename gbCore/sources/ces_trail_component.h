//
//  ces_trail_component.h
//  gbCore
//
//  Created by serhii.s on 3/28/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_trail_component : public ces_base_component
    {
    private:
        
        ui32 m_segments = 0;
        ui32 m_segments_used = 0;

        f32 m_segment_length = 0.f;
        
        glm::vec3 m_last_segment_position = glm::vec3(0.f);
        
        ui32 m_fade_out_segments = 4;
        
        f32 m_width = 1.f;
        
        f32 m_min_length = .01f;
        
    protected:

    public:
        
        CTTI_CLASS_GUID(ces_trail_component, ces_base_component::g_guids_container)
        ces_trail_component();
        ~ces_trail_component();
        
        void set_parameters(ui32 segments, f32 segment_length, f32 width);
        void set_start_position(const glm::vec3& start_position);
        
        ui32 get_segments_num() const;
        ui32 get_used_segments_num() const;
        
        f32 get_segment_length() const;
        f32 get_width() const;
    };
};

