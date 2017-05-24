//
//  ces_geometry_component.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "mesh_2d.h"

namespace gb
{
    class ces_geometry_component : public ces_base_component
    {
    private:
        
    protected:
        
        std::shared_ptr<mesh_2d<vertex_attribute>> m_mesh;
        
    public:
        
        CTTI_CLASS_GUID(ces_geometry_component, ces_base_component::g_guids_container)
        ces_geometry_component();
        ~ces_geometry_component();
        
        std::shared_ptr<mesh_2d<vertex_attribute>> get_mesh() const;
    };
};
