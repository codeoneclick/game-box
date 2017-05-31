//
//  ces_convex_hull_component.h
//  gbCore
//
//  Created by sergey.sergeev on 1/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "vbo.h"

namespace gb
{
    class ces_convex_hull_component : public ces_base_component
    {
    private:
        
    protected:
        
        glm::vec2 m_center;
        std::vector<glm::vec2> m_oriented_vertices;
        
    public:
        
        CTTI_CLASS_GUID(ces_convex_hull_component, ces_base_component::g_guids_container)
        ces_convex_hull_component();
        ~ces_convex_hull_component();
        
        void create(const vbo::vertex_attribute_PTC* vertices, i32 vertices_count);
        
        std::property_ro<glm::vec2> center;
        std::property_ro<const std::vector<glm::vec2>&> oriented_vertices;
    };
};
