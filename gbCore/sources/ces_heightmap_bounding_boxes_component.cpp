//
//  ces_heightmap_bounding_boxes_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_heightmap_bounding_boxes_component.h"

namespace gb
{
    ces_heightmap_bounding_boxes_component::ces_heightmap_bounding_boxes_component()
    {
        
    }
    
    ces_heightmap_bounding_boxes_component::~ces_heightmap_bounding_boxes_component()
    {
        
    }
    
    std::vector<std::tuple<glm::vec3, glm::vec3>>& ces_heightmap_bounding_boxes_component::get_bounding_boxes()
    {
        return m_bounding_boxes;
    }
    
    std::tuple<glm::vec3, glm::vec3> ces_heightmap_bounding_boxes_component::get_bounding_box(i32 index) const
    {
        return m_bounding_boxes.at(index);
    }
};
