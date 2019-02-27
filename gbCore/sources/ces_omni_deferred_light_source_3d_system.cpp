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
#include "ces_material_component.h"
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
            assert(material_component != nullptr);
            material_component->set_custom_shader_uniform<glm::mat4>(mat_i_vp, "u_mat_i_vp");
            material_component->set_custom_shader_uniform<glm::vec3>(transformation_component->get_position(), "u_position");
            material_component->set_custom_shader_uniform<f32>(omni_deferred_light_source_3d_component->get_radius(), "u_radius");
        });
    }
    
    void ces_omni_deferred_light_source_3d_system::on_feed_end(f32 dt)
    {
        
    }
}
