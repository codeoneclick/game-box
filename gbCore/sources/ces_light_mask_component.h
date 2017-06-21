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
    public:
        
        struct shadow_caster_metadata
        {
            ces_entity_weak_ptr m_shadow_caster;
            ui32 m_absolute_transform_matrix_version = -1;
            std::vector<glm::vec2> m_shadow_casters_vertices;
            std::vector<glm::vec4> m_shadow_casters_edges;
            bool m_is_ignored = false;
        };
        
    private:
        
        static const ui32 k_max_num_vertices;
        static const ui32 k_max_num_indices;
        static const f32 k_bounds_trashhold;
        
    protected:
        
        //std::list<glm::vec2> m_shadow_casters_vertices;
        //std::vector<glm::vec4> m_shadow_casters_edges;
        
        std::unordered_map<i32, std::shared_ptr<shadow_caster_metadata>> m_shadow_casters_metadata;
        
        std::vector<vbo::vertex_attribute> m_vertices;
        std::vector<ui16> m_indices;
        
        mesh_2d_shared_ptr m_mesh;
        
        f32 m_radius;
        glm::vec2 m_center;
        glm::vec4 m_bounds;
        
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
