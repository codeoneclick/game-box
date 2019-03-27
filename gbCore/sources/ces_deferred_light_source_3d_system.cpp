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
#include "camera_3d.h"

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
        const auto mat_i_vp = camera_3d->get_mat_i_vp();
        
        ces_base_system::enumerate_entities_with_components(m_components_mask, [=](const ces_entity_shared_ptr& entity) {
            
            const auto transformation_component = entity->get_component<ces_transformation_component>()->as_3d();
            const auto deferred_light_source_3d_component = entity->get_component<ces_deferred_light_source_3d_component>();
            const auto material_component = entity->get_component<ces_material_component>();
            const auto shader_uniforms_component = entity->get_component<ces_shader_uniforms_component>();
            const auto uniforms = shader_uniforms_component->get_uniforms();
            
            uniforms->set(mat_i_vp, ces_deferred_light_source_3d_component::k_mat_i_vp_uniform);
            uniforms->set(glm::vec2(1024, 768), ces_deferred_light_source_3d_component::k_frame_size_uniform);
            uniforms->set(transformation_component->get_absolute_position(), ces_deferred_light_source_3d_component::k_center_uniform);
            uniforms->set(deferred_light_source_3d_component->get_ray_length(), ces_deferred_light_source_3d_component::k_ray_length_uniform);
            uniforms->set(deferred_light_source_3d_component->get_color(), ces_deferred_light_source_3d_component::k_color_uniform);
            uniforms->set(glm::vec4(camera_3d->get_position(), 0.0), ces_deferred_light_source_3d_component::k_camera_position_uniform);
        });
    }
    
    void ces_deferred_light_source_3d_system::on_feed_end(f32 dt)
    {
        
    }
}
