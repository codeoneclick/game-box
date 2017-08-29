//
//  heightmap.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "heightmap.h"
#include "ces_heightmap_container_component.h"
#include "ces_heightmap_bounding_boxes_component.h"
#include "ces_heightmap_configuration_component.h"
#include "ces_heightmap_preprocessing_component.h"

namespace gb
{
    heightmap::heightmap()
    {
        ces_entity::add_deferred_component_constructor<ces_heightmap_container_component>();
        ces_entity::add_deferred_component_constructor<ces_heightmap_bounding_boxes_component>();
        ces_entity::add_deferred_component_constructor<ces_heightmap_configuration_component>();
        ces_entity::add_deferred_component_constructor<ces_heightmap_preprocessing_component>();
    }
    
    heightmap::~heightmap()
    {
        
    }
    
    void heightmap::setup_components()
    {

    }
}
