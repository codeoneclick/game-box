//
//  light.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/8/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "light.h"
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
    
    light::light() :
    m_radius(1.f),
    m_color(0.f)
    {
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_freeform_component>();
        ces_entity::add_component(geometry_component);
        unsafe_get_geometry_freeform_component_from_this->set_mesh(mesh_constructor::create_circle());
        
        ces_light_compoment_shared_ptr light_component = std::make_shared<ces_light_compoment>();
        ces_entity::add_component(light_component);
        
        ces_light_mask_component_shared_ptr light_mask_component = std::make_shared<ces_light_mask_component>();
        ces_entity::add_component(light_mask_component);
    }
    
    light::~light()
    {
        
    }
    
    void light::set_radius(f32 radius)
    {
        m_radius = radius;
        
        ces_transformation_component* transformation_component = unsafe_get_transformation_component_from_this;
        transformation_component->set_scale(glm::vec2(m_radius));
        
        ces_material_component* material_component = unsafe_get_material_component_from_this;
        material_component->set_custom_shader_uniform(m_radius, k_radius_uniform);
    }
    
    f32 light::get_radius() const
    {
        return m_radius;
    }
    
    void light::set_color(const glm::vec4& color)
    {
        m_color = color;
        
        ces_material_component* material_component = unsafe_get_material_component_from_this;
        material_component->set_custom_shader_uniform(m_color, k_color_uniform);
    }
    
    glm::vec4 light::get_color() const
    {
        return m_color;
    }
    
    void light::set_position(const glm::vec2& position)
    {
        game_object::set_position(position);
        
        glm::mat4 mat_m = glm::mat4(1.f);
        ces_entity_shared_ptr parent = ces_entity::get_parent();
        
        while(parent)
        {
            ces_transformation_component* transformation_component = unsafe_get_transformation_component(parent);
            mat_m = mat_m * transformation_component->get_matrix_m();
            parent = parent->get_parent();
        }
        glm::vec2 center = glm::transform(position, mat_m);
        ces_material_component* material_component = unsafe_get_material_component_from_this;
        material_component->set_custom_shader_uniform(center, k_center_uniform);
    }
}