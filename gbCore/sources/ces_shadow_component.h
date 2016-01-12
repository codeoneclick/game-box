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
        
        std::vector<glm::vec2> convex_hull(const vbo::vertex_attribute* vertices, i32 vertices_count);
        
        mesh_shared_ptr m_mesh;
        
    public:
        
        ces_shadow_component();
        ~ces_shadow_component();
        
        void serialize_shadow_caster_geometry(const mesh_shared_ptr& mesh);
        
        void update_shadow_geometry(const glm::vec2& light_caster_position, const glm::mat4& shadow_caster_mat_m,
                                    const vbo::vertex_attribute* shadow_caster_vertices, i32 shadow_caster_vertices_count,
                                    const ui16* shadow_caster_indices, i32 shadow_caster_indices_count);
        
        void generate_mesh();
        
        void cleanup();
        
        mesh_shared_ptr get_mesh() const;
    };
};

#endif
