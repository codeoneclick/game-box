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
#include "mesh_constructor.h"
#include "ces_font_component.h"
#include "texture_configuration.h"
#include "sprite_configuration.h"
#include "label_configuration.h"
#include "font_configuration.h"
#include "glyph_configuration.h"
#include "configuration_accessor.h"
#include "sprite.h"
#include "label.h"
#include "light_source_2d.h"
#include "ces_geometry_component.h"
#include "ces_material_extension.h"

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
    
    const configuration_accessor_shared_ptr scene_fabricator::get_configuration_accessor() const
    {
        return m_configuration_accessor;
    }
    
    const resource_accessor_shared_ptr scene_fabricator::get_resource_accessor() const
    {
        return m_resource_accessor;
    }

#if !defined(__NO_RENDER__)
    
    void scene_fabricator::add_materials(const ces_entity_shared_ptr& entity,
                                         const std::vector<std::shared_ptr<configuration>>& configurations)
    {
        for(const auto& iterator : configurations)
        {
            std::shared_ptr<material_configuration> material_configuration =
            std::static_pointer_cast<gb::material_configuration>(iterator);
            
            material_shared_ptr material = material::construct(material_configuration);
            gb::material::set_shader(material, material_configuration, m_resource_accessor);
            gb::material::set_textures(material, material_configuration, m_resource_accessor);
            ces_material_extension::add_material(entity, material_configuration->get_technique_name(),
                                                 material_configuration->get_technique_pass(), material);
        }
    }
    
#endif
    
    label_shared_ptr scene_fabricator::create_label(const std::string& filename)
    {
        auto label_configuration =
        std::static_pointer_cast<gb::label_configuration>(m_configuration_accessor->get_label_configuration(filename));
        assert(label_configuration);
        label_shared_ptr label = nullptr;
        if(label_configuration)
        {
            label = std::make_shared<gb::label>();
            
#if !defined(__NO_RENDER__)
            
            scene_fabricator::add_materials(label, label_configuration->get_materials_configurations());
            
#endif
            
            auto font_component = label->get_component<gb::ces_font_component>();
            auto font_configuration = label_configuration->get_font_configuration();
            if(font_configuration)
            {
                font_component->atlas_width = font_configuration->get_size_width();
                font_component->atlas_height = font_configuration->get_size_height();
                for(const auto& iterator : font_configuration->get_glyphs_configurations())
                {
                    auto glyph_configuration =
                    std::static_pointer_cast<gb::glyph_configuration>(iterator);
                    
                    font_component->add_glyph(glyph_configuration->get_id(),
                                              glyph_configuration->get_x(),
                                              glyph_configuration->get_y(),
                                              glyph_configuration->get_width(),
                                              glyph_configuration->get_height(),
                                              glyph_configuration->get_xoffset(),
                                              glyph_configuration->get_yoffset());
                }
            }
            else
            {
                assert(false);
            }
        }
        return label;
    }
    
    light_source_2d_shared_ptr scene_fabricator::create_light_source_2d(const std::string& filename)
    {
        std::shared_ptr<sprite_configuration> sprite_configuration =
        std::static_pointer_cast<gb::sprite_configuration>(m_configuration_accessor->get_sprite_configuration(filename));
        assert(sprite_configuration);
        light_source_2d_shared_ptr light_source = nullptr;
        if(sprite_configuration)
        {
            light_source = std::make_shared<gb::light_source_2d>();
            
#if !defined(__NO_RENDER__)
            
            scene_fabricator::add_materials(light_source, sprite_configuration->get_materials_configurations());
            
#endif
            
        }
        return light_source;
    }

	sprite_shared_ptr scene_fabricator::create_sprite(const std::string& filename)
	{
		std::shared_ptr<sprite_configuration> sprite_configuration =
			std::static_pointer_cast<gb::sprite_configuration>(m_configuration_accessor->get_sprite_configuration(filename));
		assert(sprite_configuration);
		sprite_shared_ptr sprite = nullptr;
		if (sprite_configuration)
		{
			sprite = std::make_shared<gb::sprite>();

#if !defined(__NO_RENDER__)

			scene_fabricator::add_materials(sprite, sprite_configuration->get_materials_configurations());

#endif

		}
		return sprite;
	}
}
