//
//  ces_geometry_component.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_geometry_component : public ces_base_component
    {
    public:
        
        enum e_bounding_mode
        {
            e_none = -1,
            e_point = 0,
            e_radius,
            e_box
        };
        
    private:
        
    protected:
        
        e_bounding_mode m_bounding_mode = e_none;
        glm::vec4 m_bounding_box;
        f32 m_bounding_radius = 0.f;
        ui32 m_bounds_version;
        mesh_2d_shared_ptr m_mesh;
        
    public:
        
        STTI_CLASS_GUID(ces_geometry_component, ces_base_component::g_guids_container)
        ces_geometry_component();
        ~ces_geometry_component();
        
        std::property_rw<e_bounding_mode> bounding_mode;
        
        mesh_2d_shared_ptr get_mesh() const;
        
        void set_bounding_box(const glm::vec4& bounds, ui32 bounds_version);
        glm::vec4 get_bounding_box() const;
        
        void set_bounding_radius(f32 radius);
        f32 get_bounding_radius() const;
        
        ui32 get_bounds_version() const;
    };
};
