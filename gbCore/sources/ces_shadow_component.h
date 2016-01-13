//
//  ces_shadow_component.h
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_shadow_component_h
#define ces_shadow_component_h

#include "ces_base_component.h"
#include "vbo.h"

namespace gb
{
    class ces_shadow_component : public ces_base_component
    {
    private:
        
    protected:
        
        mesh_shared_ptr m_mesh;
        
    public:
        
        ces_shadow_component();
        ~ces_shadow_component();
        
        void update_shadow_geometry(const glm::vec2& light_caster_position, const glm::mat4& shadow_caster_mat_m,
                                    const std::vector<glm::vec2>& convex_hull_oriented_vertices);

        mesh_shared_ptr get_shadow_mesh() const;
    };
};

#endif
