//
//  scene_fabricator.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "scene_fabricator.h"
#include "resource_accessor.h"
#include "mesh_2d.h"
#include "material.h"
#include "mesh_constructor.h"
#include "ces_font_component.h"
#include "texture_configuration.h"
#include "sprite_configuration.h"
#include "label_configuration.h"
#include "font_configuration.h"
#include "glyph_configuration.h"
#include "shape_3d_configuration.h"
#include "animation_3d_sequence_configuration.h"
#include "configuration_accessor.h"
#include "sprite.h"
#include "shape_3d.h"
#include "label.h"
#include "heightmap.h"
#include "light_source_2d.h"
#include "heightmap_mmap.h"
#include "ces_geometry_component.h"
#include "ces_material_extension.h"
#include "ces_animation_3d_mixer_component.h"
#include "ces_animation_3d_mask_component.h"
#include "ces_skeleton_3d_component.h"
#include "ces_geometry_3d_component.h"
#include "ces_heightmap_container_component.h"
#include "ces_heightmap_configuration_component.h"
#include "ces_heightmap_accessor_component.h"
#include "mesh_3d.h"
#include "mesh_3d_loading_operation.h"
#include "animation_3d_sequence.h"
#include "ces_animation_3d_system.h"
#include "animation_3d_sequence_loading_operation.h"
#include "ces_heightmap_chunks_component.h"

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

#if USED_GRAPHICS_API != NO_GRAPHICS_API
    
    bool scene_fabricator::is_using_batch(const std::vector<std::shared_ptr<configuration>>& configurations)
    {
        for(const auto& iterator : configurations)
        {
            std::shared_ptr<material_configuration> material_configuration = std::static_pointer_cast<gb::material_configuration>(iterator);
            if(!material_configuration->get_is_batching())
            {
                return false;
            }
        }
        return true;
    }
    
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
    
    void scene_fabricator::add_animation_sequences_3d(const ces_entity_shared_ptr& entity,
                                                      const mesh_3d_shared_ptr& mesh,
                                                      const std::vector<std::shared_ptr<configuration>>& configurations)
    {
        auto animation_3d_mixer_compoment = entity->get_component<ces_animation_3d_mixer_component>();
        if (!animation_3d_mixer_compoment)
        {
            animation_3d_mixer_compoment = std::make_shared<ces_animation_3d_mixer_component>();
            entity->add_component(animation_3d_mixer_compoment);
        }
        auto skeleton_3d_component = entity->get_component<ces_skeleton_3d_component>();
        if (!skeleton_3d_component)
        {
            skeleton_3d_component = std::make_shared<ces_skeleton_3d_component>();
            entity->add_component(skeleton_3d_component);
        }
        
        animation_3d_mixer_compoment->setup(mesh->get_skeleton_data(),
                                            mesh->get_bindpose_data());
        skeleton_3d_component->setup(mesh->get_skeleton_data(),
                                    mesh->get_bindpose_data());

        
        ces_animation_3d_system::bind_pose(animation_3d_mixer_compoment, skeleton_3d_component);

        for (const auto& iterator : configurations)
        {
            std::shared_ptr<animation_3d_sequence_configuration> animation_3d_sequence_configuration =
            std::static_pointer_cast<gb::animation_3d_sequence_configuration>(iterator);
            
            auto animation_sequence = m_resource_accessor->get_resource<animation_3d_sequence, animation_3d_sequence_loading_operation>(animation_3d_sequence_configuration->get_animation_filename(), true);
            
            i32 animation_start_index = animation_3d_sequence_configuration->get_start_index();
            i32 animation_end_index = animation_3d_sequence_configuration->get_end_index();
            if (animation_sequence->get_num_frames() > animation_start_index &&
                animation_sequence->get_num_frames() > animation_end_index &&
                animation_start_index <= animation_end_index)
            {
                std::vector<frame_3d_data_shared_ptr> frames;
                for (i32 i = animation_start_index; i <= animation_end_index; ++i)
                {
                    frames.push_back(animation_sequence->get_frame(i));
                }
                std::stringstream string_stream_guid;
                string_stream_guid<<animation_sequence->get_guid()<<"_"<<animation_3d_sequence_configuration->get_start_index()<<"_"<<animation_3d_sequence_configuration->get_end_index();
                
                const auto sequence_data = std::make_shared<sequence_3d_transfering_data>(animation_3d_sequence_configuration->get_animation_name(), 30, frames);
                animation_sequence = animation_3d_sequence::construct(string_stream_guid.str(), sequence_data);
                
            }
            else
            {
                assert(false && "wrong start or end index in animation");
            }

            animation_3d_mixer_compoment->add_animation_sequence(animation_sequence);
            
            const auto animation_mask_configuration_filename = animation_3d_sequence_configuration->get_animation_mask_filename();
            if (animation_mask_configuration_filename.length() != 0)
            {
                auto animation_3d_mask_compoment = entity->get_component<ces_animation_3d_mask_component>();
                if (!animation_3d_mask_compoment)
                {
                    animation_3d_mask_compoment = std::make_shared<ces_animation_3d_mask_component>();
                    entity->add_component(animation_3d_mask_compoment);
                }
                
                const auto animation_mask_configuration = std::static_pointer_cast<gb::animation_3d_mask_configuration>(m_configuration_accessor->get_animation_3d_mask_configuration(animation_mask_configuration_filename));
                const auto bones_mask_configurations = animation_mask_configuration->get_bone_mask_configurations();
                for (const auto& bone_mask_iterator : bones_mask_configurations)
                {
                    const auto animation_bone_mask_configurations = std::static_pointer_cast<gb::animation_3d_bone_mask_configuration>(bone_mask_iterator);
                    const auto bone_name = animation_bone_mask_configurations->get_bone_name();
                    const auto bone_weight = animation_bone_mask_configurations->get_weight();
                    animation_3d_mask_compoment->add_bone_mask(animation_3d_sequence_configuration->get_animation_name(),
                                                               bone_name, bone_weight);
                }
            }
        }
    }
    
    label_shared_ptr scene_fabricator::create_label(const std::string& filename)
    {
        auto label_configuration =
        std::static_pointer_cast<gb::label_configuration>(m_configuration_accessor->get_label_configuration(filename));
        assert(label_configuration);
        label_shared_ptr label = nullptr;
        if(label_configuration)
        {
            label = gb::ces_entity::construct<gb::label>();
            
#if USED_GRAPHICS_API != NO_GRAPHICS_API
            
            scene_fabricator::add_materials(label, label_configuration->get_materials_configurations());
            
#endif
        
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
            light_source = gb::ces_entity::construct<gb::light_source_2d>();
            
#if USED_GRAPHICS_API != NO_GRAPHICS_API
            
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
            bool is_using_batch = scene_fabricator::is_using_batch(sprite_configuration->get_materials_configurations());
            sprite = gb::ces_entity::construct<gb::sprite>(is_using_batch);
            
#if USED_GRAPHICS_API != NO_GRAPHICS_API
            
            scene_fabricator::add_materials(sprite, sprite_configuration->get_materials_configurations());
            
#endif
            
        }
        return sprite;
    }
    
    shape_3d_shared_ptr scene_fabricator::create_shape_3d(const std::string& filename)
    {
        std::shared_ptr<shape_3d_configuration> shape_3d_configuration =
        std::static_pointer_cast<gb::shape_3d_configuration>(m_configuration_accessor->get_shape_3d_configuration(filename));
        assert(shape_3d_configuration);
        shape_3d_shared_ptr shape_3d = nullptr;
        if (shape_3d_configuration)
        {
            shape_3d = gb::ces_entity::construct<gb::shape_3d>();
            
            auto mesh = m_resource_accessor->get_resource<mesh_3d, mesh_3d_loading_operation>(shape_3d_configuration->get_mesh_filename(), true);
            auto geometry_3d_component = shape_3d->get_component<ces_geometry_3d_component>();
            geometry_3d_component->set_mesh(mesh);
            
#if USED_GRAPHICS_API != NO_GRAPHICS_API
            
            scene_fabricator::add_materials(shape_3d, shape_3d_configuration->get_materials_configurations());
            
#endif
            if (shape_3d_configuration->get_animations_configurations().size() > 0)
            {
                scene_fabricator::add_animation_sequences_3d(shape_3d, mesh, shape_3d_configuration->get_animations_configurations());
            }
        }
        return shape_3d;
    }
    
    heightmap_shared_ptr scene_fabricator::create_heightmap(const std::string& filename)
    {
        auto heightmap_configuration = std::static_pointer_cast<gb::heightmap_configuration>(m_configuration_accessor->get_heightmap_configuration(filename));
        heightmap_shared_ptr heightmap = nullptr;
        if(heightmap_configuration)
        {
            heightmap = ces_entity::construct<gb::heightmap>();
            auto heightmap_mmap = std::make_shared<gb::heightmap_mmap>(heightmap_configuration->get_heightmap_data_filename());
            
            const auto& heightmap_container_component = heightmap->get_component<ces_heightmap_container_component>();
            heightmap_container_component->setup(heightmap_mmap, glm::ivec2(heightmap_configuration->get_heightmap_chunk_size_x(),
                                                                            heightmap_configuration->get_heightmap_chunk_size_y()));
            
            const auto& heightmap_configuration_component = heightmap->get_component<ces_heightmap_configuration_component>();
            heightmap_configuration_component->setup(heightmap_configuration->get_splatting_diffuse_texture_preprocessing_material_filename(),
                                                     heightmap_configuration->get_heightmap_chunk_material_filename());
            
            const auto& heightmap_accessor_component = heightmap->get_component<ces_heightmap_accessor_component>();
            heightmap_accessor_component->setup(heightmap_mmap);
            
            const auto& heightmap_chunks_component = heightmap->get_component<ces_heightmap_chunks_component>();
            heightmap_chunks_component->setup(heightmap_container_component->get_chunks_count());
        }
        return heightmap;
    }
}
