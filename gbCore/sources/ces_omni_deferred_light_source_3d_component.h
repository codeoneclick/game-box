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
    class ces_omni_deferred_light_source_3d_component : public ces_base_component
    {
    private:
        
        f32 m_radius;
        glm::vec4 m_color;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_omni_deferred_light_source_3d_component, ces_base_component::g_guids_container)
        ces_omni_deferred_light_source_3d_component() = default;
        ~ces_omni_deferred_light_source_3d_component() = default;
        
        void set_radius(f32 radius);
        void set_color(const glm::vec4& color);
        
        f32 get_radius() const;
        glm::vec4 get_color() const;
    };
};
