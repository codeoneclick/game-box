//
//  ces_convex_hull_component.h
//  gbCore
//
//  Created by sergey.sergeev on 1/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_convex_hull_component_h
#define ces_convex_hull_component_h

#include "ces_base_component.h"
#include "vbo.h"

namespace gb
{
    class ces_convex_hull_component : public ces_base_component
    {
    private:
        
    protected:
        
        std::vector<glm::vec2> m_oriented_vertices;
        
    public:
        
        ces_convex_hull_component();
        ~ces_convex_hull_component();
        
        void create_convex_hull(const vbo::vertex_attribute* vertices, i32 vertices_count);
        const std::vector<glm::vec2>& get_oriented_vertices() const;
    };
};

#endif
