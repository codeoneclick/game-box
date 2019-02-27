//
//  omni_deffered_light_source.cpp
//  gbCore
//
//  Created by serhii.s on 2/26/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "omni_deferred_light_source_3d.h"
#include "ces_geometry_3d_component.h"
#include "mesh_constructor.h"
#include "ces_transformation_3d_component.h"
#include "ces_material_component.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_extension.h"
#include "ces_omni_deferred_light_source_3d_component.h"
#include "glm_extensions.h"

namespace gb
{
    static const std::string k_radius_uniform = "u_radius";
    static const std::string k_center_uniform = "u_center";
    static const std::string k_color_uniform = "u_color";
    
    omni_deferred_light_source_3d::omni_deferred_light_source_3d()
    {
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        ces_entity::add_deferred_component_constructor<ces_material_component>();
        
#endif
        
        ces_entity::add_deferred_component_constructor<ces_geometry_3d_component>();
        ces_entity::add_deferred_component_constructor<ces_omni_deferred_light_source_3d_component>();
        
        radius.setter([=](f32 radius) {
            const auto omni_deferred_light_source_3d_component = ces_entity::get_component<ces_omni_deferred_light_source_3d_component>();
            omni_deferred_light_source_3d_component->set_radius(radius);
            const auto transformation_component = ces_entity::get_component<ces_transformation_3d_component>();
            transformation_component->set_scale(glm::vec3(radius));
            
#if USED_GRAPHICS_API != NO_GRAPHICS_API
            
            //const auto material_component = ces_entity::get_component<ces_material_component>();
            //material_component->set_custom_shader_uniform(radius, k_radius_uniform);
            
#endif
            
        });
        radius.getter([=]() {
            const auto omni_deferred_light_source_3d_component = ces_entity::get_component<ces_omni_deferred_light_source_3d_component>();
            return omni_deferred_light_source_3d_component->get_radius();
        });
        
        color.setter([=](const glm::vec4& color) {
            const auto omni_deferred_light_source_3d_component = ces_entity::get_component<ces_omni_deferred_light_source_3d_component>();
            omni_deferred_light_source_3d_component->set_color(color);
            
#if USED_GRAPHICS_API != NO_GRAPHICS_API
            
            //const auto material_component = ces_entity::get_component<ces_material_component>();
            //material_component->set_custom_shader_uniform(color, k_color_uniform);
            
#endif
            
        });
        color.getter([=]() {
            const auto omni_deferred_light_source_3d_component = ces_entity::get_component<ces_omni_deferred_light_source_3d_component>();
            return omni_deferred_light_source_3d_component->get_color();
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
            
            glm::vec3 center = glm::transform(position, transformation_component->get_matrix_m());
            
#if USED_GRAPHICS_API != NO_GRAPHICS_API
            
            //const auto material_component = ces_entity::get_component<ces_material_component>();
            //material_component->set_custom_shader_uniform(center, k_center_uniform);
            
#endif
        });
    }
    
    omni_deferred_light_source_3d::~omni_deferred_light_source_3d()
    {
        
    }
    
    void omni_deferred_light_source_3d::setup_components()
    {
        const auto geometry_component = ces_entity::get_component<ces_geometry_3d_component>();
        geometry_component->set_mesh(mesh_constructor::create_sphere());
    }
}
