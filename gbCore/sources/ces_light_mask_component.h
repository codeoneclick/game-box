//
//  ces_light_mask_component.h
//  gbCore
//
//  Created by sergey.sergeev on 1/18/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "mesh_2d.h"

namespace gb
{
    class ces_light_mask_component : public ces_base_component
    {
        
    private:
        
        static const ui32 k_max_num_vertices;
        static const ui32 k_max_num_indices;
        static const f32 k_bounds_trashhold;
        static const i32 k_max_intersections_count = 1024;
        
    protected:
        
        std::set<f32> m_unique_raytrace_angles;
        std::vector<glm::vec4> m_shadow_casters_edges;
        std::vector<vbo::vertex_attribute> m_vertices;
        std::vector<ui16> m_indices;
        std::array<glm::vec3, k_max_intersections_count> m_intersections;
        i32 m_used_intersections;
        
        mesh_2d_shared_ptr m_mesh;
        
        f32 m_radius;
        glm::vec2 m_center;
        glm::vec4 m_light_mask_bounds;
        std::array<glm::vec2, 4> m_light_mask_bounds_geometry;
        
        void update_bounds();
        
    public:
        
        CTTI_CLASS_GUID(ces_light_mask_component, ces_base_component::g_guids_container)
        ces_light_mask_component();
        ~ces_light_mask_component();
        
        std::property_rw<f32> radius;
        std::property_rw<glm::vec2> center;
        
        void add_shadowcasters_geometry(i32 id,
                                        const ces_entity_shared_ptr& shadow_caster,
                                        ui32 absolute_transform_matrix_version,
                                        const glm::mat4& shadow_caster_mat_m,
                                        const std::vector<glm::vec2>& convex_hull_oriented_vertices);
        void update_mesh();
        
        mesh_2d_shared_ptr get_mesh() const;
        
        void cleanup();
    };
};
