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
    private:
        
    protected:
        
        glm::vec4 m_bounds;
        ui32 m_bounds_version;
        mesh_2d_shared_ptr m_mesh;
        
    public:
        
        CTTI_CLASS_GUID(ces_geometry_component, ces_base_component::g_guids_container)
        ces_geometry_component();
        ~ces_geometry_component();
        
        mesh_2d_shared_ptr get_mesh() const;
        
        void set_bounds(const glm::vec4& bounds, ui32 bounds_version);
        glm::vec4 get_bounds() const;
        ui32 get_bounds_version() const;
    };
};
