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
        
        typedef std::function<void(const std::vector<ces_entity_weak_ptr>& entities_inside, const std::vector<ces_entity_weak_ptr>& entities_outside)> inside_outside_result_callback_t;
        
    private:
        
        static const ui32 k_max_num_vertices;
        static const ui32 k_max_num_indices;
        static const f32 k_bounds_trashhold;
        static const i32 k_max_intersections_count = 1024;
        
    protected:
        
        std::queue<std::pair<std::vector<ces_entity_weak_ptr>, inside_outside_result_callback_t>> m_inside_outside_requests_queue;
        std::mutex m_inside_outside_request_mutex;
        
        std::set<f32> m_unique_sorted_raytrace_angles;
        std::vector<glm::vec4> m_shadow_casters_edges;
        
        std::vector<vbo::vertex_attribute> m_vertices;
        std::vector<ui16> m_indices;
        
        std::array<glm::vec2, k_max_intersections_count> m_intersections;
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
        
        void add_shadowcasters_geometry(const std::vector<glm::vec2>& convex_hull_oriented_vertices);
        void update_mesh();
        
        mesh_2d_shared_ptr get_mesh() const;
        
        void cleanup();
        
        void push_inside_outside_request(const std::vector<ces_entity_weak_ptr>& entities_source,
                                         const inside_outside_result_callback_t& callback);
        std::pair<std::vector<ces_entity_weak_ptr>, inside_outside_result_callback_t> pop_inside_outside_request();
        bool is_inside_outside_requests_exist() const;
    };
};
