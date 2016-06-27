//
//  ces_light_mask_component.h
//  gbCore
//
//  Created by sergey.sergeev on 1/18/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef ces_light_mask_component_h
#define ces_light_mask_component_h

#include "ces_base_component.h"
#include "mesh.h"

namespace gb
{
    class ces_light_mask_component : public ces_base_component
    {
        
    private:
        
    protected:
        
        std::vector<glm::vec2> m_shadow_casters_vertices;
        std::vector<std::pair<glm::vec2, glm::vec2>> m_shadow_casters_edges;
        std::vector<vbo::vertex_attribute> m_vertices;
        std::vector<ui16> m_indices;
        
    public:
        
        CTTI_GUID_H(ces_light_mask_component)
        ces_light_mask_component();
        ~ces_light_mask_component();
        
        void update_mask_geometry(const glm::mat4& shadow_caster_mat_m,
                                  const std::vector<glm::vec2>& convex_hull_oriented_vertices);
        
        void generate_mask_mesh(const glm::vec2& light_caster_position);
        
        mesh_shared_ptr get_mask_mesh() const;
        
        void cleanup();
    };
    
#define unsafe_get_light_mask_component(entity) static_cast<ces_light_mask_component*>(entity->get_component(ces_light_mask_component::class_guid()).get())
#define unsafe_get_light_mask_component_from_this static_cast<ces_light_mask_component*>(ces_entity::get_component(ces_light_mask_component::class_guid()).get())
    
};

#endif
