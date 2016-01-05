//
//  direction_light.cpp
//  gbCore
//
//  Created by sergey.sergeev on 9/22/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "direction_light.h"
#include "ces_transformation_component.h"
#include "ces_geometry_component.h"
#include "ces_render_component.h"
#include "material.h"

namespace gb
{
    direction_light::direction_light() :
    m_direction(glm::vec3(0.f, 1.f, 0.f)),
    m_color(glm::vec4(1.f)),
    m_intensity(1.f)
    {
        ces_transformation_component_shared_ptr transformation_component = std::make_shared<ces_transformation_component>();
        ces_entity::add_component(transformation_component);
        
        ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_component>();
        ces_entity::add_component(geometry_component);
    }
    
    direction_light::~direction_light()
    {
        
    }
    
    void direction_light::set_direction(const glm::vec3& direction)
    {
        m_direction = direction;
        unsafe_get_render_component_from_this->set_custom_shader_uniform(m_direction,
                                                                         "u_light_direction");
        
    }
    
    glm::vec3 direction_light::get_direction() const
    {
        return m_direction;
    }
    
    void direction_light::set_color(const glm::vec4& color)
    {
        m_color = color;
        unsafe_get_render_component_from_this->set_custom_shader_uniform(m_color,
                                                                         "u_light_color");
    }
    
    glm::vec4 direction_light::get_color() const
    {
        return m_color;
    }
    
    void direction_light::set_intensity(f32 intensity)
    {
        m_intensity = std::max(std::min(1.f, intensity), 0.f);
        unsafe_get_render_component_from_this->set_custom_shader_uniform(m_intensity,
                                                                         "u_light_intensity");
    }
    
    f32 direction_light::get_intensity() const
    {
        return m_intensity;
    }
    
    void direction_light::add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material)
    {
        unsafe_get_render_component_from_this->add_material(technique_name, technique_pass, material);
        
        unsafe_get_render_component_from_this->set_custom_shader_uniform(m_intensity, "u_light_intensity");
        unsafe_get_render_component_from_this->set_custom_shader_uniform(m_color, "u_light_color");
        unsafe_get_render_component_from_this->set_custom_shader_uniform(m_direction, "u_light_direction");
    }
    
    void direction_light::set_mesh(const mesh_shared_ptr& mesh)
    {
        unsafe_get_geometry_component_from_this->set_mesh(mesh);
    }
}