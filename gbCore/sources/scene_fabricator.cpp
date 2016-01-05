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

namespace gb
{
    scene_fabricator::scene_fabricator()
    {
        
    }
    
    scene_fabricator::~scene_fabricator()
    {
        
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
}