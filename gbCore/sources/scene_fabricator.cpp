//
//  scene_fabricator.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "scene_fabricator.h"
#include "resource_accessor.h"
#include "mesh.h"
#include "material.h"
#include "renderable_game_object.h"
#include "mesh_constructor.h"
#include "texture_configuration.h"
#include "shape_configuration.h"
#include "configuration_accessor.h"
#include "static_shape.h"
#include "ces_geometry_component.h"

namespace gb
{
    scene_fabricator::scene_fabricator()
    {
        
    }
    
    scene_fabricator::~scene_fabricator()
    {
        
    }
    
    void scene_fabricator::set_configuration_accessor(const configuration_accessor_shared_ptr& configuration_accessor)
    {
        m_configuration_accessor = configuration_accessor;
    }
    
    void scene_fabricator::set_resource_accessor(const resource_accessor_shared_ptr& resource_accessor)
    {
        m_resource_accessor = resource_accessor;
    }
    
    void scene_fabricator::add_materials(const renderable_game_object_shared_ptr& renderable_game_object,
                                         const std::vector<std::shared_ptr<configuration>>& configurations)
    {
        for(const auto& iterator : configurations)
        {
            std::shared_ptr<material_configuration> material_configuration =
            std::static_pointer_cast<gb::material_configuration>(iterator);
            
            material_shared_ptr material = material::construct(material_configuration);
            gb::material::set_shader(material, material_configuration, m_resource_accessor);
            gb::material::set_textures(material, material_configuration, m_resource_accessor);
            renderable_game_object->add_material(material_configuration->get_technique_name(),
                                                 material_configuration->get_technique_pass(), material);
        }
    }
    
    static_shape_shared_ptr scene_fabricator::create_static_shape(const std::string& filename)
    {
        std::shared_ptr<shape_configuration> shape_configuration =
        std::static_pointer_cast<gb::shape_configuration>(m_configuration_accessor->get_shape_configuration(filename));
        assert(shape_configuration);
        static_shape_shared_ptr static_shape = nullptr;
        if(shape_configuration)
        {
            static_shape = std::make_shared<gb::static_shape>();
            
            mesh_shared_ptr mesh = mesh_constructor::create_shape_quad();
            assert(mesh);
            
            ces_geometry_component* geometry_component = unsafe_get_geometry_component(static_shape);
            assert(geometry_component);
            geometry_component->set_mesh(mesh);
            
            scene_fabricator::add_materials(static_shape, shape_configuration->get_materials_configurations());
            m_game_objects_container.insert(static_shape);
        }
        return static_shape;
    }
}