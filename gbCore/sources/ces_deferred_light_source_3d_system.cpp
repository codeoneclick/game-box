//
//  ces_omni_deferred_light_source_3d_system.cpp
//  gbCore
//
//  Created by serhii.s on 2/27/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_deferred_light_source_3d_system.h"
#include "ces_deferred_light_source_3d_component.h"
#include "ces_transformation_3d_component.h"
#include "ces_shader_uniforms_component.h"
#include "ces_material_component.h"
#include "ces_geometry_component.h"
#include "camera_3d.h"
#include "mesh_3d.h"
#include "glm_extensions.h"

namespace gb
{
    ces_deferred_light_source_3d_system::ces_deferred_light_source_3d_system()
    {
        ces_base_system::add_required_component_guid(m_components_mask, ces_deferred_light_source_3d_component::class_guid());
        ces_base_system::add_required_components_mask(m_components_mask);
    }
    
    void ces_deferred_light_source_3d_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_deferred_light_source_3d_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        const auto camera_3d = ces_base_system::get_current_camera_3d();
        
#if defined(__DYNAMIC__CULLING_MODE__)
        
        const auto camera_3d_position = camera_3d->get_position();
        
#endif
        
        ces_base_system::enumerate_entities_with_components(m_components_mask, [=](const ces_entity_shared_ptr& entity) {
            
            const auto transformation_component = entity->get_component<ces_transformation_component>()->as_3d();
            const auto deferred_light_source_3d_component = entity->get_component<ces_deferred_light_source_3d_component>();
            const auto material_component = entity->get_component<ces_material_component>();
            const auto shader_uniforms_component = entity->get_component<ces_shader_uniforms_component>();
            
#if defined(__DYNAMIC__CULLING_MODE__)
            
            const auto geometry_component = entity->get_component<ces_geometry_component>();
            const auto mesh = geometry_component->get_mesh();
            
            const auto radius = geometry_component->get_bounding_radius();
            const auto position = transformation_component->get_absolute_position();
            const auto distance = glm::distance(position, camera_3d_position);
            if (distance <= radius)
            {
                 material_component->set_culling_mode(gl::constant::back);
            }
            else
            {
                material_component->set_culling_mode(gl::constant::front);
            }
            const auto max_bound = mesh->as_3d()->get_max_bound();
            const auto min_bound = mesh->as_3d()->get_min_bound();
            const auto mat_m = transformation_component->get_absolute_transformation();
            glm::vec3 absolute_min_bound = glm::transform(min_bound, mat_m);
            glm::vec3 absolute_max_bound = glm::transform(max_bound, mat_m);
            if (camera_3d_position.x > absolute_min_bound.x && camera_3d_position.x < absolute_max_bound.x &&
                camera_3d_position.y > absolute_min_bound.y && camera_3d_position.y < absolute_max_bound.y &&
                camera_3d_position.z > absolute_min_bound.z && camera_3d_position.z < absolute_max_bound.z)
            {
                material_component->set_culling_mode(gl::constant::back);
            }
            else
            {
                material_component->set_culling_mode(gl::constant::front);
            }
            
#endif
            
            auto uniforms = shader_uniforms_component->get_uniforms(ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment, "deferred_point_light", 0);
            if (uniforms)
            {
                uniforms->set(transformation_component->get_absolute_position(), ces_deferred_light_source_3d_component::k_light_position_uniform);
                uniforms->set(deferred_light_source_3d_component->get_direction(), ces_deferred_light_source_3d_component::k_light_direction_uniform);
                uniforms->set(deferred_light_source_3d_component->get_ray_length(), ces_deferred_light_source_3d_component::k_light_ray_length_uniform);
                uniforms->set(deferred_light_source_3d_component->get_outer_cutoff_angle(), ces_deferred_light_source_3d_component::k_light_outer_cutoff_angle_uniform);
                uniforms->set(deferred_light_source_3d_component->get_inner_cutoff_angle(), ces_deferred_light_source_3d_component::k_light_inner_cutoff_angle_uniform);
                uniforms->set(deferred_light_source_3d_component->get_color(), ces_deferred_light_source_3d_component::k_light_color_uniform);
                uniforms->set(glm::vec4(camera_3d->get_position(), 0.0), ces_deferred_light_source_3d_component::k_camera_position_uniform);
            }
            
            uniforms = shader_uniforms_component->get_uniforms(ces_shader_uniforms_component::e_shader_uniform_mode::e_fragment, "deferred_spot_light", 0);
            
            if (uniforms)
            {
                uniforms->set(transformation_component->get_absolute_position(), ces_deferred_light_source_3d_component::k_light_position_uniform);
                uniforms->set(deferred_light_source_3d_component->get_direction(), ces_deferred_light_source_3d_component::k_light_direction_uniform);
                uniforms->set(deferred_light_source_3d_component->get_ray_length(), ces_deferred_light_source_3d_component::k_light_ray_length_uniform);
                uniforms->set(deferred_light_source_3d_component->get_outer_cutoff_angle(), ces_deferred_light_source_3d_component::k_light_outer_cutoff_angle_uniform);
                uniforms->set(deferred_light_source_3d_component->get_inner_cutoff_angle(), ces_deferred_light_source_3d_component::k_light_inner_cutoff_angle_uniform);
                uniforms->set(deferred_light_source_3d_component->get_color(), ces_deferred_light_source_3d_component::k_light_color_uniform);
                uniforms->set(glm::vec4(camera_3d->get_position(), 0.0), ces_deferred_light_source_3d_component::k_camera_position_uniform);
            }
        });
    }
    
    void ces_deferred_light_source_3d_system::on_feed_end(f32 dt)
    {
        
    }
}
