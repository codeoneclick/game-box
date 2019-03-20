//
//  ces_omni_deferred_light_source_3d_system.cpp
//  gbCore
//
//  Created by serhii.s on 2/27/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_omni_deferred_light_source_3d_system.h"
#include "ces_omni_deferred_light_source_3d_component.h"
#include "ces_transformation_3d_component.h"
#include "ces_shader_uniforms_component.h"
#include "ces_material_component.h"
#include "omni_deferred_light_source_3d.h"
#include "camera_3d.h"

namespace gb
{
    ces_omni_deferred_light_source_3d_system::ces_omni_deferred_light_source_3d_system()
    {
        ces_base_system::add_required_component_guid(m_components_mask, ces_omni_deferred_light_source_3d_component::class_guid());
        ces_base_system::add_required_components_mask(m_components_mask);
    }
    
    void ces_omni_deferred_light_source_3d_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_omni_deferred_light_source_3d_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
    {
        const auto camera_3d = ces_base_system::get_current_camera_3d();
        const auto mat_i_vp = camera_3d->get_mat_i_vp();
        
        ces_base_system::enumerate_entities_with_components(m_components_mask, [=](const ces_entity_shared_ptr& entity) {
            
            const auto transformation_component = entity->get_component<ces_transformation_component>()->as_3d();
            const auto omni_deferred_light_source_3d_component = entity->get_component<ces_omni_deferred_light_source_3d_component>();
            const auto material_component = entity->get_component<ces_material_component>();
            const auto shader_uniforms_component = entity->get_component<ces_shader_uniforms_component>();
            const auto uniforms = shader_uniforms_component->get_uniforms_as<omni_deferred_light_source_3d::shader_uniforms>();
            
            uniforms->set_mat_i_vp(mat_i_vp);
            uniforms->set_frame_size(glm::vec2(1920, 1080));
            uniforms->set_center(transformation_component->get_position());
            uniforms->set_radius(omni_deferred_light_source_3d_component->get_radius());
            uniforms->set_color(omni_deferred_light_source_3d_component->get_color());
        });
    }
    
    void ces_omni_deferred_light_source_3d_system::on_feed_end(f32 dt)
    {
        
    }
}
