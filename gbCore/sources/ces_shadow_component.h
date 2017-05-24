//
//  ces_shadow_component.h
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "mesh_2d.h"
#include "vbo.h"

namespace gb
{
    class ces_shadow_component : public ces_base_component
    {
    private:
        
    protected:
        
        std::shared_ptr<mesh_2d<vertex_attribute>> m_mesh;
        
        std::vector<vertex_attribute> m_vertices;
        std::vector<ui16> m_indices;
        
    public:
        
        CTTI_CLASS_GUID(ces_shadow_component, ces_base_component::g_guids_container)
        ces_shadow_component();
        ~ces_shadow_component();
        
        void update_shadow_geometry(const glm::vec2& light_caster_position, const glm::mat4& shadow_caster_mat_m,
                                    const std::vector<glm::vec2>& convex_hull_oriented_vertices);

        void generate_shadow_mesh();
        
        std::shared_ptr<mesh_2d<vertex_attribute>> get_shadow_mesh() const;
        
        void cleanup();
    };
};
