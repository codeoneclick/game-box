//
//  omni_deffered_light_source.cpp
//  gbCore
//
//  Created by serhii.s on 2/26/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "deferred_light_source_3d.h"
#include "ces_geometry_3d_component.h"
#include "mesh_constructor.h"
#include "ces_transformation_3d_component.h"
#include "ces_material_component.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_extension.h"
#include "ces_deferred_light_source_3d_component.h"
#include "glm_extensions.h"

namespace gb
{
    deferred_light_source_3d::deferred_light_source_3d()
    {
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        ces_entity::add_deferred_component_constructor<ces_material_component>();
        
#endif
        
        ces_entity::add_deferred_component_constructor<ces_geometry_3d_component>();
        ces_entity::add_deferred_component_constructor<ces_deferred_light_source_3d_component>();
        ces_entity::add_deferred_component_constructor<ces_shader_uniforms_component>();
        
        color.setter([=](const glm::vec4& color) {
            const auto deferred_light_source_3d_component = ces_entity::get_component<ces_deferred_light_source_3d_component>();
            deferred_light_source_3d_component->set_color(color);
        });
        color.getter([=]() {
            const auto deferred_light_source_3d_component = ces_entity::get_component<ces_deferred_light_source_3d_component>();
            return deferred_light_source_3d_component->get_color();
        });
        
        position.setter([=](const glm::vec3& position) {
            const auto box2d_body_component = ces_entity::get_component<ces_box2d_body_component>();
            if(box2d_body_component && box2d_body_component->is_applied)
            {
                box2d_body_component->position = glm::vec2(position.x, position.z);
            }
            
            const auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            transformation_component->set_position(position);
            ces_transformation_extension::update_absolute_transformation_recursively(shared_from_this());
        });
    }
    
    deferred_light_source_3d::~deferred_light_source_3d()
    {
        
    }
    
    void deferred_light_source_3d::setup_components()
    {
        
    }
}
