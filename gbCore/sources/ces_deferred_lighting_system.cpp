//
//  ces_deferred_lighting_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#if !defined(__NO_RENDER__)

#include "ces_deferred_lighting_system.h"
#include "ces_light_compoment.h"
#include "ces_geometry_component.h"
#include "ces_shadow_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_convex_hull_component.h"
#include "ces_material_component.h"
#include "ces_light_mask_component.h"
#include "ces_luminous_component.h"
#include "mesh_2d.h"
#include "glm_extensions.h"

namespace gb
{
    ces_deferred_lighting_system::ces_deferred_lighting_system()
    {

    }
    
    ces_deferred_lighting_system::~ces_deferred_lighting_system()
    {
        ces_deferred_lighting_system::cleanup();
    }
    
    void ces_deferred_lighting_system::on_feed_start(f32 deltatime)
    {
        ces_deferred_lighting_system::cleanup();
    }
    
    void ces_deferred_lighting_system::cleanup()
    {
        m_light_casters.clear();
        m_shadow_casters.clear();
    }
    
    void ces_deferred_lighting_system::on_feed(ces_entity_const_shared_ptr entity, f32 deltatime)
    {
        ces_deferred_lighting_system::update_recursively(entity);
    }
    
    void ces_deferred_lighting_system::on_feed_end(f32 deltatime)
    {
        for(const auto& weak_light_caster : m_light_casters)
        {
            if(!weak_light_caster.expired())
            {
                ces_entity_shared_ptr light_caster = weak_light_caster.lock();
                auto light_component = light_caster->get_component<ces_light_compoment>();
                light_component->cleanup();
                
                auto light_mask_component = light_caster->get_component<ces_light_mask_component>();
                light_mask_component->cleanup();
                
                auto light_caster_transformation_component = light_caster->get_component<ces_transformation_2d_component>();
                
                glm::mat4 light_caster_mat_m = light_caster_transformation_component->get_absolute_transformation();
                light_mask_component->center = glm::vec2(light_caster_mat_m[3][0],
                                                         light_caster_mat_m[3][1]);
                light_mask_component->radius = light_caster_transformation_component->get_scale().x;
                
                for(const auto& weak_shadow_caster : m_shadow_casters)
                {
                    if(!weak_shadow_caster.expired())
                    {
                        ces_entity_shared_ptr shadow_caster = weak_shadow_caster.lock();
                        
                        const auto& convex_hull_component = shadow_caster->get_component<ces_convex_hull_component>();
                        const auto& shadow_caster_transformation_component = shadow_caster->get_component<ces_transformation_2d_component>();
                        const auto& shadow_component = shadow_caster->get_component<ces_shadow_component>();
                        
                        const std::vector<glm::vec2>& oriented_vertices = convex_hull_component->oriented_vertices;
                        light_mask_component->add_shadowcasters_geometry(shadow_component->get_id(),
                                                                         shadow_caster,
                                                                         shadow_caster_transformation_component->get_absolute_matrix_version(),
                                                                         shadow_caster_transformation_component->get_absolute_transformation(),
                                                                         oriented_vertices);
                    }
                }
                light_mask_component->update_mesh();
            }
        }
    }
    
    void ces_deferred_lighting_system::update_recursively(ces_entity_const_shared_ptr entity)
    {
        if(entity->is_component_exist<ces_light_compoment>())
        {
            m_light_casters.insert(entity);
        }
        
        if(entity->is_component_exist<ces_shadow_component>())
        {
            m_shadow_casters.insert(entity);
        }
        
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_deferred_lighting_system::update_recursively(child);
        }
    }
}

#endif
