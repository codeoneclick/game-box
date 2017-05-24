//
//  ces_geometry_freeform_component.hpp
//  gbCore
//
//  Created by sergey.sergeev on 1/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_geometry_component.h"

namespace gb
{
    class ces_geometry_freeform_component : public ces_geometry_component
    {
    private:
        
    protected:
        
    public:
        
        ces_geometry_freeform_component();
        ~ces_geometry_freeform_component();
        
        void set_mesh(const std::shared_ptr<mesh_2d<vertex_attribute>>& mesh);
    };
};
