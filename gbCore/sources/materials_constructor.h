//
//  materials_constructor.h
//  gbCore
//
//  Created by sergey.sergeev on 12/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef materials_constructor_h
#define materials_constructor_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class materials_constructor
    {
    private:
        
    protected:
        
        static shader_shared_ptr get_direction_light_shader();
        static shader_shared_ptr get_omni_light_shader();
        static shader_shared_ptr get_wireframe_grid_shader();
        static shader_shared_ptr get_3d_cursor_shader();
        
    public:
        
        materials_constructor() = default;
        ~materials_constructor() = default;
        
        static material_shared_ptr create_direction_light_material();
        static material_shared_ptr create_omni_light_material();
        static material_shared_ptr create_wireframe_grid_material();
        static material_shared_ptr create_3d_cursor_material();
    };
};

#endif
