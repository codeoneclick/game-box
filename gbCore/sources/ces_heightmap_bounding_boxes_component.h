//
//  ces_heightmap_bounding_boxes_component.h
//  gbCore
//
//  Created by serhii serhiiv on 8/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_heightmap_bounding_boxes_component : public ces_base_component
    {
    private:
        
        std::vector<std::tuple<glm::vec3, glm::vec3>> m_bounding_boxes;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_heightmap_bounding_boxes_component, ces_base_component::g_guids_container)
        ces_heightmap_bounding_boxes_component();
        ~ces_heightmap_bounding_boxes_component();
        
        std::vector<std::tuple<glm::vec3, glm::vec3>>& get_bounding_boxes();
    };
};
