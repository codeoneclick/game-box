//
//  shape_3d.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/16/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "shape_3d.h"
#include "ces_material_component.h"
#include "ces_geometry_3d_component.h"
#include "ces_animation_3d_mixer_component.h"
#include "ces_skeleton_3d_component.h"

namespace gb
{
    shape_3d::shape_3d()
    {
#if !defined(__NO_RENDER__)
        
        auto material_component = std::make_shared<ces_material_component>();
        ces_entity::add_component(material_component);
        
#endif
        
        auto geometry_component = std::make_shared<ces_geometry_3d_component>();
        ces_entity::add_component(geometry_component);
        
        auto animation_3d_mixer_component = std::make_shared<ces_animation_3d_mixer_component>();
        ces_entity::add_component(animation_3d_mixer_component);
        
        auto skeleton_3d_component = std::make_shared<ces_skeleton_3d_component>();
        ces_entity::add_component(skeleton_3d_component);
    }
    
    shape_3d::~shape_3d()
    {
        
    }
}
