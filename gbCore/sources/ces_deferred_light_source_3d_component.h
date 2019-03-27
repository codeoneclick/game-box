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
        
        static const std::string k_mat_i_vp_uniform;
        static const std::string k_frame_size_uniform;
        static const std::string k_ray_length_uniform;
        static const std::string k_center_uniform;
        static const std::string k_color_uniform;
        static const std::string k_camera_position_uniform;
        
    private:
        
        f32 m_ray_length = 1.f;
        glm::vec4 m_color = glm::vec4(1.0);
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_deferred_light_source_3d_component, ces_base_component::g_guids_container)
        ces_deferred_light_source_3d_component() = default;
        ~ces_deferred_light_source_3d_component() = default;
        
        void set_ray_length(f32 ray_length);
        void set_color(const glm::vec4& color);
        
        f32 get_ray_length() const;
        glm::vec4 get_color() const;
    };
};
