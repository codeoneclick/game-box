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
        f32 m_segment_width = 0.f;
        f32 m_min_segment_length = .01f;
        
        glm::vec3 m_old_segment_position = glm::vec3(0.f);
        glm::vec3 m_new_segment_position = glm::vec3(0.f);
        glm::vec3 m_old_segment_direction = glm::vec3(0.f);
        glm::vec3 m_new_segment_direction = glm::vec3(0.f);
        
        f32 m_emitt_timestamp;
        f32 m_max_visible_time = 5000.f;
        bool m_is_enabled = false;
        
        std::shared_ptr<std::vector<f32>> m_segment_timestamps = nullptr;
        
    protected:

    public:
        
        STTI_CLASS_GUID(ces_trail_component, ces_base_component::g_guids_container)
        ces_trail_component();
        ~ces_trail_component();
        
        void set_parameters(ui32 segments, f32 segment_length, f32 segment_width);
        void set_start_position(const glm::vec3& position);
        
        ui32 get_segments_num() const;
        
        ui32 get_used_segments_num() const;
        void set_used_segments_num(ui32 segments);
        
        f32 get_segment_length() const;
        f32 get_segment_width() const;
        
        void set_old_segment_position(const glm::vec3 position);
        glm::vec3 get_old_segment_position() const;
        
        void set_new_segment_position(const glm::vec3 position);
        glm::vec3 get_new_segment_position() const;
        
        void set_old_segment_direction(const glm::vec3& direction);
        glm::vec3 get_old_segment_direction() const;
        
        void set_new_segment_direction(const glm::vec3& direction);
        glm::vec3 get_new_segment_direction() const;
        
        f32 get_min_segment_length() const;
        
        f32 get_max_visible_time() const;
        
        std::shared_ptr<std::vector<f32>> get_segment_timestamps() const;
        
        void set_enabled(bool value);
        bool get_enabled() const;
        
        bool is_expired() const;
    };
};

