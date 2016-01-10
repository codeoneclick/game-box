//
//  ces_scene_component.cpp
//  gbCore
//
//  Created by Sergey Sergeev on 1/10/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_scene_component.h"

namespace gb
{
    ces_scene_component::ces_scene_component()
    {
        m_type = e_ces_component_type_scene;
    }
    
    ces_scene_component::~ces_scene_component()
    {
        
    }
    
    void ces_scene_component::set_scene(const scene_graph_shared_ptr& scene)
    {
        m_scene = scene;
    }
}