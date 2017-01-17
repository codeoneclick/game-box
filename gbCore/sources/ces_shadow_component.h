//
//  ces_shadow_component.h
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "ces_base_component.h"
#include "vbo.h"

namespace gb
{
    class ces_shadow_component : public ces_base_component
    {
    private:
        
    protected:
        
        mesh_shared_ptr m_mesh;
        
        std::vector<vbo::vertex_attribute> m_vertices;
        std::vector<ui16> m_indices;
        
    public:
        
        CTTI_CLASS_GUID(ces_shadow_component, ces_base_component::g_guids_container)
        ces_shadow_component();
        ~ces_shadow_component();
        
        void update_shadow_geometry(const glm::vec2& light_caster_position, const glm::mat4& shadow_caster_mat_m,
                                    const std::vector<glm::vec2>& convex_hull_oriented_vertices);

        void generate_shadow_mesh();
        
        mesh_shared_ptr get_shadow_mesh() const;
        
        void cleanup();
    };
};

#endif