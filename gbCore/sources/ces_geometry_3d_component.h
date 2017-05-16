//
//  ces_geometry_3d_component.h
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_geometry_component.h"

namespace gb
{
    class ces_geometry_3d_component : public ces_geometry_component
    {
    private:
        
    protected:
        
    public:
        
        ces_geometry_3d_component();
        ~ces_geometry_3d_component();
        
        void set_mesh(const mesh_3d_shared_ptr& mesh);
    };
};
