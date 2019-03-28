//
//  ces_omni_deferred_light_source_3d_component.h
//  gbCore
//
//  Created by serhii.s on 2/27/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//
#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_deferred_light_source_3d_component : public ces_base_component
    {
    public:
        
        static const std::string k_light_ray_length_uniform;
        static const std::string k_light_position_uniform;
        static const std::string k_light_direction_uniform;
        static const std::string k_light_cutoff_angle_uniform;
        static const std::string k_light_inner_cutoff_angle_uniform;
        static const std::string k_light_color_uniform;
        static const std::string k_camera_position_uniform;
        
    private:
        
        
        f32 m_ray_length = 1.f;
        glm::vec4 m_color = glm::vec4(1.0);
        glm::vec3 m_direction = glm::vec3(0.f, -1.f, 0.f);
        f32 m_cutoff_angle = 0.f;
        f32 m_inner_cutoff_angle = 0.f;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_deferred_light_source_3d_component, ces_base_component::g_guids_container)
        ces_deferred_light_source_3d_component() = default;
        ~ces_deferred_light_source_3d_component() = default;
        
        void set_ray_length(f32 ray_length);
        void set_direction(const glm::vec3& direction);
        void set_cutoff_angle(f32 angle);
        void set_inner_cutoff_angle(f32 angle);
        void set_color(const glm::vec4& color);
        
        f32 get_ray_length() const;
        glm::vec3 get_direction() const;
        f32 get_cutoff_angle() const;
        f32 get_inner_cutoff_angle() const;
        glm::vec4 get_color() const;
    };
};
