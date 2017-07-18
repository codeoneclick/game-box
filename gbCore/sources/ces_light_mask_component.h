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
        
    protected:
        
        std::queue<std::pair<std::vector<ces_entity_weak_ptr>, inside_outside_result_callback_t>> m_inside_outside_requests_queue;
        std::mutex m_inside_outside_request_mutex;
        
        std::queue<std::tuple<glm::vec2, std::array<glm::vec2, 4>, std::vector<std::vector<glm::vec2>>>> m_shadow_geometry_to_apply_queue;
        std::mutex m_shadow_geometry_apply_mutex;
        

        bool m_is_uploaded_to_vram;
        mesh_2d_shared_ptr m_mesh;
        
        f32 m_radius;
        glm::vec2 m_center;
        glm::vec4 m_bounds;
        std::array<glm::vec2, 4> m_bounds_geometry;
        
        void update_bounds();
        
    public:
        
        CTTI_CLASS_GUID(ces_light_mask_component, ces_base_component::g_guids_container)
        ces_light_mask_component();
        ~ces_light_mask_component();
        
        std::property_rw<f32> radius;
        std::property_rw<glm::vec2> center;
        
        bool is_shadowcaster_affect(const std::vector<glm::vec2>& convex_hull_oriented_vertices) const;
        
        void push_shadowcasters_geometry(const std::vector<std::vector<glm::vec2>>& shadowcasters_geometry);
        void apply_shadowcasters_geometry();
        
        mesh_2d_shared_ptr get_mesh() const;
        
        void push_inside_outside_request(const std::vector<ces_entity_weak_ptr>& entities_source,
                                         const inside_outside_result_callback_t& callback);
        std::pair<std::vector<ces_entity_weak_ptr>, inside_outside_result_callback_t> pop_inside_outside_request();
        bool is_inside_outside_requests_exist() const;
    };
};
