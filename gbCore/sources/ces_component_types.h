//
//  ces_component_types.h
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_component_types_h
#define ces_component_types_h

namespace gb
{
    enum ces_component_type
    {
        k_ces_component_type_undefined = 0,
        k_ces_component_type_transformation,
        k_ces_component_type_render,
        k_ces_component_type_geometry,
        k_ces_component_type_animation,
        k_ces_component_type_particle_emitter,
        k_ces_component_type_touch,
        k_ces_component_type_max
    };
};

#endif
