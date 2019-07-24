//
//  mesh_constructor.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class mesh_constructor
    {
    private:
        
        static mesh_3d_shared_ptr m_shared_box_mesh;
        static mesh_3d_shared_ptr m_shared_sphere_mesh;
        static mesh_2d_shared_ptr m_shared_screen_quad_mesh;
        
    protected:
        
    public:
        
        mesh_constructor() = default;
        ~mesh_constructor() = default;
        
        static mesh_2d_shared_ptr create_screen_quad();
        static mesh_2d_shared_ptr create_shape_quad(bool is_use_batch = false);
        static mesh_2d_shared_ptr create_circle();
        
        static mesh_3d_shared_ptr create_plane_3d();
        static mesh_3d_shared_ptr create_shared_box();
        static mesh_3d_shared_ptr create_box();
        static mesh_3d_shared_ptr create_shared_sphere();
        static mesh_3d_shared_ptr create_sphere(ui32 sectors = 16, ui32 rings = 16);
        static mesh_3d_shared_ptr create_trail(ui32 segments);
    };
};
