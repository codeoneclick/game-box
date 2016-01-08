//
//  mesh_constructor.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef mesh_constructor_h
#define mesh_constructor_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class mesh_constructor
    {
    private:
        
    protected:
        
    public:
        
        mesh_constructor() = default;
        ~mesh_constructor() = default;
        
        static mesh_shared_ptr create_screen_quad();
        static mesh_shared_ptr create_shape_quad();
        static mesh_shared_ptr create_circle();
    };
};

#endif
