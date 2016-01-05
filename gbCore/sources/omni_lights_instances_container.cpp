//
//  omni_lights_instances_container.cpp
//  gbCore
//
//  Created by sergey.sergeev on 12/14/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "omni_lights_instances_container.h"
#include "shader.h"
#include "material.h"
#include "mesh_constructor.h"
#include "omni_light.h"
#include "instanced_mesh.h"

namespace gb
{
    const ui32 omni_lights_instances_container::k_max_lights_in_container = 64;
    
    omni_lights_instances_container::omni_lights_instances_container() :
    m_material(nullptr),
    m_mesh(nullptr)
    {
        m_transformations = std::make_shared<std::vector<glm::vec4>>();
        m_colors = std::make_shared<std::vector<glm::vec4>>();
        
        m_transformations->resize(k_max_lights_in_container, glm::vec4(0.f, 0.f, 0.f, 1.f));
        m_colors->resize(k_max_lights_in_container, glm::vec4(1.f, 1.f, 1.f, 1.f));
    }
    
    omni_lights_instances_container::~omni_lights_instances_container()
    {
        
    }
    
    void omni_lights_instances_container::add_omni_light(const omni_light_shared_ptr& omni_light)
    {
        m_lights.insert(omni_light);
        
        omni_light->set_instance_id(static_cast<i32>(m_lights.size() - 1));
        omni_light->set_external_uniforms_data(m_transformations, m_colors);
        
        omni_light->add_material(m_technique_name, 0, m_material);
        omni_light->set_mesh(m_mesh);
        
        m_mesh->set_num_instances(static_cast<i32>(m_lights.size()));
    }
    
    void omni_lights_instances_container::remove_omni_light(const omni_light_shared_ptr& omni_light)
    {
        m_lights.erase(omni_light);
    }
    
    ui32 omni_lights_instances_container::get_omni_lights_count() const
    {
        return static_cast<ui32>(m_lights.size());
    }
    
    void omni_lights_instances_container::set_material(const material_shared_ptr& material, const std::string& technique_name)
    {
        m_material = material;
        m_technique_name = technique_name;
    }
    
    void omni_lights_instances_container::set_mesh(const instanced_mesh_shared_ptr& mesh)
    {
        m_mesh = mesh;
    }
}