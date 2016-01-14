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
    enum e_ces_component_type
    {
        e_ces_component_type_undefined = 0,
        e_ces_component_type_transformation,
        e_ces_component_type_material,
        e_ces_component_type_geometry,
        e_ces_component_type_text,
        e_ces_component_type_scene,
        e_ces_component_type_light,
        e_ces_component_type_shadow,
        e_ces_component_type_convex_hull,
        e_ces_component_type_bound_touch,
        e_ces_component_type_max
    };
};

#endif
