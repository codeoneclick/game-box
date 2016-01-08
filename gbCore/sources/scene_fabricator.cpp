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
#include "sprite_configuration.h"
#include "configuration_accessor.h"
#include "sprite.h"
#include "label.h"
#include "light.h"
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
    
    sprite_shared_ptr scene_fabricator::create_sprite(const std::string& filename)
    {
        std::shared_ptr<sprite_configuration> sprite_configuration =
        std::static_pointer_cast<gb::sprite_configuration>(m_configuration_accessor->get_sprite_configuration(filename));
        assert(sprite_configuration);
        sprite_shared_ptr sprite = nullptr;
        if(sprite_configuration)
        {
            sprite = std::make_shared<gb::sprite>();
            
            scene_fabricator::add_materials(sprite, sprite_configuration->get_materials_configurations());
            m_game_objects_container.insert(sprite);
        }
        return sprite;
    }
    
    label_shared_ptr scene_fabricator::create_label(const std::string& filename)
    {
        std::shared_ptr<sprite_configuration> sprite_configuration =
        std::static_pointer_cast<gb::sprite_configuration>(m_configuration_accessor->get_sprite_configuration(filename));
        assert(sprite_configuration);
        label_shared_ptr label = nullptr;
        if(sprite_configuration)
        {
            label = std::make_shared<gb::label>();
            
            scene_fabricator::add_materials(label, sprite_configuration->get_materials_configurations());
            m_game_objects_container.insert(label);
        }
        return label;
    }
    
    light_shared_ptr scene_fabricator::create_light(const std::string& filename)
    {
        std::shared_ptr<sprite_configuration> sprite_configuration =
        std::static_pointer_cast<gb::sprite_configuration>(m_configuration_accessor->get_sprite_configuration(filename));
        assert(sprite_configuration);
        light_shared_ptr light = nullptr;
        if(sprite_configuration)
        {
            light = std::make_shared<gb::light>();
            
            scene_fabricator::add_materials(light, sprite_configuration->get_materials_configurations());
            m_game_objects_container.insert(light);
        }
        return light;
    }
}