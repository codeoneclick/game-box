//
//  ces_light_mask_component.h
//  gbCore
//
//  Created by sergey.sergeev on 1/18/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "ces_base_component.h"
#include "mesh.h"

namespace gb
{
    class ces_light_mask_component : public ces_base_component
    {
        
    private:
        
        static const ui32 k_max_num_vertices;
        static const ui32 k_max_num_indices;
        
    protected:
        
        std::vector<glm::vec2> m_shadow_casters_vertices;
        std::vector<std::pair<glm::vec2, glm::vec2>> m_shadow_casters_edges;
        std::vector<vbo::vertex_attribute> m_vertices;
        std::vector<ui16> m_indices;
        
        mesh_shared_ptr m_mesh;
        
    public:
        
        CTTI_CLASS_GUID(ces_light_mask_component, ces_base_component::g_guids_container)
        ces_light_mask_component();
        ~ces_light_mask_component();
        
        void update_mask_geometry(const glm::mat4& shadow_caster_mat_m,
                                  const std::vector<glm::vec2>& convex_hull_oriented_vertices);
        
        void generate_mask_mesh(const glm::vec2& light_caster_position);
        
        mesh_shared_ptr get_mask_mesh() const;
        
        void cleanup();
    };
};

#endif
