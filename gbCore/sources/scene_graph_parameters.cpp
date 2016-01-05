//
//  scene_graph_parameters.cpp
//  gbCore
//
//  Created by sergey.sergeev on 12/16/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "scene_graph_parameters.h"

namespace gb
{
    scene_graph_parameters::scene_graph_parameters() :
    m_main_camera(nullptr),
    m_shadow_cast_light(nullptr)
    {
        
    }
    
    scene_graph_parameters::~scene_graph_parameters()
    {
        
    }
}