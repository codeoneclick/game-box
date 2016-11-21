//
//  ces_deferred_lighting_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_deferred_lighting_system.h"
#include "ces_light_compoment.h"
#include "ces_geometry_component.h"
#include "ces_shadow_component.h"
#include "ces_transformation_component.h"
#include "ces_convex_hull_component.h"
#include "ces_material_component.h"
#include "ces_light_mask_component.h"
#include "ces_shadow_emissive_component.h"
#include "mesh.h"
#include "glm_extensions.h"

namespace gb
{
    ces_deferred_lighting_system::ces_deferred_lighting_system()
    {

    }
    
    ces_deferred_lighting_system::~ces_deferred_lighting_system()
    {
        
    }
    
    void ces_deferred_lighting_system::on_feed_start(f32 deltatime)
    {
        m_light_casters.clear();
        m_shadow_casters.clear();
        m_shadow_emissive_entities.clear();
    }
    
    void ces_deferred_lighting_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
    {
        ces_deferred_lighting_system::update_recursively(entity);
    }
    
    void ces_deferred_lighting_system::on_feed_end(f32 deltatime)
    {
        for(const auto& light_caster : m_light_casters)
        {
            ces_light_compoment* light_component = unsafe_get_light_component(light_caster);
            light_component->cleanup();
            
            ces_light_mask_component* light_mask_component = unsafe_get_light_mask_component(light_caster);
            light_mask_component->cleanup();
            
            ces_transformation_component* light_caster_transformation_component = unsafe_get_transformation_component(light_caster);
            
            glm::mat4 light_caster_mat_m = glm::mat4(1.f);
            ces_entity_shared_ptr parent = light_caster->parent;
            
            while(parent)
            {
                ces_transformation_component* transformation_component = unsafe_get_transformation_component(parent);
                light_caster_mat_m = light_caster_mat_m * transformation_component->get_matrix_m();
                parent = parent->parent;
            }

            glm::vec2 light_caster_position = glm::transform(light_caster_transformation_component->get_position(), light_caster_mat_m);
            
            for(const auto& shadow_caster : m_shadow_casters)
            {
                ces_convex_hull_component* convex_hull_component = unsafe_get_convex_hull_component(shadow_caster);
                ces_transformation_component* shadow_caster_transformation_component = unsafe_get_transformation_component(shadow_caster);
                
                glm::mat4 shadow_caster_mat_m = glm::mat4(1.f);
                ces_entity_shared_ptr parent = shadow_caster->parent;
                
                while(parent)
                {
                    ces_transformation_component* transformation_component = unsafe_get_transformation_component(parent);
                    shadow_caster_mat_m = shadow_caster_mat_m * transformation_component->get_matrix_m();
                    parent = parent->parent;
                }
                
                shadow_caster_mat_m = shadow_caster_mat_m * shadow_caster_transformation_component->get_matrix_m();
                
                light_mask_component->update_mask_geometry(shadow_caster_mat_m, convex_hull_component->get_oriented_vertices());
                
                
            }
            
            for(const auto& entity : m_shadow_emissive_entities)
            {
                light_component->add_shadow_emissive_entity(entity);
            }
            
            light_mask_component->generate_mask_mesh(light_caster_position);
        }
    }
    
    void ces_deferred_lighting_system::update_recursively(const ces_entity_shared_ptr& entity)
    {
        if(entity->is_component_exist(ces_light_compoment::class_guid()))
        {
            m_light_casters.insert(entity);
        }
        
        if(entity->is_component_exist(ces_shadow_component::class_guid()))
        {
            m_shadow_casters.insert(entity);
        }
        
        if(entity->is_component_exist(ces_shadow_emissive_component::class_guid()))
        {
            m_shadow_emissive_entities.insert(entity);
        }
        
        std::list<ces_entity_shared_ptr> children = entity->children;
        for(const auto& child : children)
        {
            ces_deferred_lighting_system::update_recursively(child);
        }
    }
}
