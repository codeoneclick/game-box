//
//  ces_deferred_lighting_system.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_deferred_lighting_system.h"
#include "ces_light_compoment.h"
#include "ces_shadow_component.h"
#include "ces_geometry_component.h"
#include "mesh.h"
#include "ces_transformation_component.h"
#include "ces_convex_hull_component.h"
#include "ces_material_component.h"
#include "glm_extensions.h"

namespace gb
{
    static const std::string k_shadow_cast_position_uniform = "u_shadow_cast_position";
    
    ces_deferred_lighting_system::ces_deferred_lighting_system()
    {
        m_type = e_ces_system_type_deferred_lighting;
    }
    
    ces_deferred_lighting_system::~ces_deferred_lighting_system()
    {
        
    }
    
    void ces_deferred_lighting_system::on_feed_start(f32 deltatime)
    {
        m_light_casters.clear();
        m_shadow_casters.clear();
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
            
            for(const auto& shadow_caster : m_shadow_casters)
            {
                ces_shadow_component* shadow_component = unsafe_get_shadow_component(shadow_caster);
                ces_convex_hull_component* convex_hull_component = unsafe_get_convex_hull_component(shadow_caster);
                ces_transformation_component* shadow_caster_transformation_component = unsafe_get_transformation_component(shadow_caster);
                ces_transformation_component* light_caster_transformation_component = unsafe_get_transformation_component(light_caster);
                
                glm::mat4 shadow_caster_mat_m = glm::mat4(1.f);
                ces_entity_shared_ptr parent = shadow_caster->get_parent();
                
                while(parent)
                {
                    ces_transformation_component* transformation_component = unsafe_get_transformation_component(parent);
                    shadow_caster_mat_m = shadow_caster_mat_m * transformation_component->get_matrix_m();
                    parent = parent->get_parent();
                }
                
                shadow_caster_mat_m = shadow_caster_mat_m * shadow_caster_transformation_component->get_matrix_m();
                
                glm::mat4 light_caster_mat_m = glm::mat4(1.f);
                parent = light_caster->get_parent();
                
                while(parent)
                {
                    ces_transformation_component* transformation_component = unsafe_get_transformation_component(parent);
                    light_caster_mat_m = light_caster_mat_m * transformation_component->get_matrix_m();
                    parent = parent->get_parent();
                }
                
                shadow_component->update_shadow_geometry(glm::transform(light_caster_transformation_component->get_position(), light_caster_mat_m),
                                                         shadow_caster_mat_m,
                                                         convex_hull_component->get_oriented_vertices());
                
                unsafe_get_material_component(shadow_caster)->set_custom_shader_uniform(convex_hull_component->get_center(), k_shadow_cast_position_uniform);
                
                light_component->add_shadow_caster(shadow_caster);
            }
        }
    }
    
    void ces_deferred_lighting_system::update_recursively(const ces_entity_shared_ptr& entity)
    {
        ces_light_compoment *light_component = unsafe_get_light_component(entity);
        
        if(light_component)
        {
            m_light_casters.insert(entity);
        }
        
        ces_shadow_component *shadow_component = unsafe_get_shadow_component(entity);
        if(shadow_component)
        {
            m_shadow_casters.insert(entity);
        }
        
        std::list<ces_entity_shared_ptr> children = entity->get_children();
        for(const auto& child : children)
        {
            ces_deferred_lighting_system::update_recursively(child);
        }
    }
}