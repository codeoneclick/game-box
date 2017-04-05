//
//  light_source.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/8/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "light_source.h"
#include "ces_geometry_freeform_component.h"
#include "ces_light_compoment.h"
#include "mesh_constructor.h"
#include "ces_transformation_component.h"
#include "ces_material_component.h"
#include "ces_light_mask_component.h"
#include "glm_extensions.h"

namespace gb
{
    static const std::string k_radius_uniform = "u_radius";
    static const std::string k_center_uniform = "u_center";
    static const std::string k_color_uniform = "u_color";
    
    light_source::light_source() :
    m_radius(1.f),
    m_color(0.f)
    {
        
#if !defined(__NO_RENDER__)
        
        auto material_component = std::make_shared<ces_material_component>();
        ces_entity::add_component(material_component);
        
#endif
        
        auto geometry_component = std::make_shared<ces_geometry_freeform_component>();
        ces_entity::add_component(geometry_component);
        geometry_component->set_mesh(mesh_constructor::create_circle());
        
        auto light_component = std::make_shared<ces_light_compoment>();
        ces_entity::add_component(light_component);
        
        auto light_mask_component = std::make_shared<ces_light_mask_component>();
        ces_entity::add_component(light_mask_component);
        
        auto transformation_component = ces_entity::get_component<ces_transformation_component>();
        
        radius.setter([=](f32 radius) {
            m_radius = radius;
            transformation_component->set_scale(glm::vec2(m_radius));

#if !defined(__NO_RENDER__)
            
            material_component->set_custom_shader_uniform(m_radius, k_radius_uniform);
            
#endif
            
        });
        radius.getter([=]() {
            return m_radius;
        });
        
        color.setter([=](const glm::vec4& color) {
            m_color = color;
            
#if !defined(__NO_RENDER__)
            
            material_component->set_custom_shader_uniform(m_color, k_color_uniform);
            
#endif
            
        });
        color.getter([=]() {
            return m_color;
        });
        
        position.setter([=](const glm::vec2& position) {
            transformation_component->set_position(position);
            glm::mat4 mat_m = glm::mat4(1.f);
            ces_entity_shared_ptr parent = ces_entity::parent;
            
            while(parent)
            {
                auto parent_transformation_component = parent->get_unsafe_component<ces_transformation_component>();
                mat_m = mat_m * parent_transformation_component->get_matrix_m();
                parent = parent->parent;
            }
            glm::vec2 center = glm::transform(position, mat_m);
            
#if !defined(__NO_RENDER__)
            
            material_component->set_custom_shader_uniform(center, k_center_uniform);
            
#endif
            
        });
    }
    
    light_source::~light_source()
    {
        
    }
}
