//
//  heightmap_textures_generator.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "heightmap_textures_generator.h"
#include "ces_heightmap_container_component.h"
#include "ces_heightmap_accessor_component.h"
#include "ces_heightmap_preprocessing_component.h"
#include "ces_heightmap_configuration_component.h"
#include "ces_heightmap_splatting_texture_preprocessing_component.h"
#include "ces_render_target_component.h"
#include "heightmap_mmap.h"
#include "heightmap_constants.h"
#include "ces_entity.h"
#include "material.h"
#include "texture.h"
#include "render_target.h"
#include "configuration_accessor.h"

namespace gb
{
    void heightmap_textures_generator::create_splatting_texture(const ces_entity_shared_ptr& entity,
                                                                i32 i, i32 j, const std::function<void(i32 current, i32 total)>& callback)
    {
        const auto& heightmap_preprocessing_component = entity->get_component<ces_heightmap_preprocessing_component>();
        for(i32 k = 0; k < heightmap_constants::e_heightmap_lod_max; ++k)
        {
            auto chunk = ces_entity::construct<ces_entity>();
            auto heightmap_splatting_texture_preprocessing_component = std::make_shared<ces_heightmap_splatting_texture_preprocessing_component>();
            auto render_target_component = std::make_shared<ces_render_target_component>();
            render_target_component->setup(heightmap_preprocessing_component->get_render_traget(static_cast<heightmap_constants::e_heightmap_lod>(k)),
                                           heightmap_preprocessing_component->get_splatting_preprocessing_material());
            
            render_target_component->set_on_render_begin_callback([entity, i, j, k](const ces_entity_shared_ptr& chunk) {
                const auto& heightmap_preprocessing_component = entity->get_component<ces_heightmap_preprocessing_component>();
                const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
                auto splatting_mask_texture = heightmap_preprocessing_component->get_splatting_mask_texture_buffer();
                splatting_mask_texture->bind();
                
                glm::ivec2 splatting_mask_texture_size = glm::ivec2(sqrt(heightmap_container_component->get_splatting_mask_textures_mmap(0)->get_size()));
                i32 index = i + j * heightmap_container_component->get_chunks_count().x;
                gl_texture_image2d(GL_TEXTURE_2D, 0, GL_RGB,
                                   splatting_mask_texture_size.x, splatting_mask_texture_size.y,
                                   0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5,
                                   heightmap_container_component->get_splatting_mask_textures_mmap(index)->get_pointer());
            });
            
            render_target_component->set_on_render_end_callback([entity, i, j, k, callback](const ces_entity_shared_ptr& chunk) {
                const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
                const auto& heightmap_preprocessing_component = entity->get_component<ces_heightmap_preprocessing_component>();
                glm::ivec2 diffuse_texture_size = heightmap_container_component->get_textures_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k));
                ui32 size = static_cast<ui32>(diffuse_texture_size.x) * static_cast<ui32>(diffuse_texture_size.y) * heightmap_constants::k_splatting_texture_channels;
                ui8 *data = new ui8[size];
                auto render_target = heightmap_preprocessing_component->get_render_traget(static_cast<heightmap_constants::e_heightmap_lod>(k));
                render_target->grab(data);
                
                ui32 size_565 = size / heightmap_constants::k_splatting_texture_channels;
                ui16* data_565 = new ui16[size_565];
                ui32 iterations_index = 0;
                for(ui32 iteration = 0; iteration < size; iteration += 4)
                {
                    data_565[iterations_index] = TO_RGB565(data[iteration + 0], data[iteration + 1], data[iteration + 2]);
                    iterations_index++;
                }
                std::shared_ptr<std::ofstream> stream = heightmap_preprocessing_component->get_splatting_processing_stream();
                stream->write((char *)data_565, size_565 * sizeof(ui16));
                delete [] data;
                delete [] data_565;
                
                entity->remove_child(chunk);
                
                heightmap_preprocessing_component->dec_executed_preprocessing_operations_count();
                glm::ivec2 chunks_count = heightmap_container_component->get_chunks_count();
                i32 total_preprocessing_operations_count = chunks_count.x * chunks_count.y * heightmap_constants::e_heightmap_lod_max;
                callback(total_preprocessing_operations_count - heightmap_preprocessing_component->get_executed_preprocessing_operations_count(),
                         total_preprocessing_operations_count);
                if(heightmap_preprocessing_component->get_executed_preprocessing_operations_count() == 0)
                {
                   stream->close();
                }
            });
            chunk->add_component(heightmap_splatting_texture_preprocessing_component);
            chunk->add_component(render_target_component);
            entity->add_child(chunk);
            heightmap_preprocessing_component->inc_executed_preprocessing_operations_count();
        }
    }
    
    void heightmap_textures_generator::create_splatting_mask_textures(const ces_entity_shared_ptr& entity)
    {
        const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
        const auto& heightmap_accessor_component = entity->get_component<ces_heightmap_accessor_component>();
        const auto& heightmap_mmap = heightmap_container_component->get_mmap();
        
        glm::ivec2 chunks_count = heightmap_container_component->get_chunks_count();
        glm::ivec2 chunk_size = heightmap_container_component->get_chunk_size();
        
        std::shared_ptr<std::ofstream> stream = std::make_shared<std::ofstream>();
        stream->open(heightmap_mmap::get_splatting_textures_mask_mmap_filename(heightmap_mmap->get_filename()), std::ios::binary | std::ios::out | std::ios::trunc);
        if(!stream->is_open())
        {
            assert(false);
        }
        
        for(ui32 i = 0; i < chunks_count.x; ++i)
        {
            for(ui32 j = 0; j < chunks_count.y; ++j)
            {
                glm::ivec2 vertices_offset(i * (chunk_size.x - 1), j * (chunk_size.y - 1));
                glm::vec2 step = glm::vec2(static_cast<f32>(chunk_size.x) / static_cast<f32>(heightmap_constants::k_splatting_texture_mask_size.x) ,
                                           static_cast<f32>(chunk_size.y) / static_cast<f32>(heightmap_constants::k_splatting_texture_mask_size.y));
                
                glm::vec3 offset = glm::vec3(0.f);
                f32 max_height = heightmap_constants::k_max_splatting_texture_height;
                
                ui16* pixels = new ui16[heightmap_constants::k_splatting_texture_mask_size.x * heightmap_constants::k_splatting_texture_mask_size.y];
                
                for(ui32 x = 0; x < heightmap_constants::k_splatting_texture_mask_size.x; ++x)
                {
                    offset.z = 0.f;
                    for(ui32 y = 0; y < heightmap_constants::k_splatting_texture_mask_size.y; ++y)
                    {
                        ui32 index = x + y * heightmap_constants::k_splatting_texture_mask_size.x;
                        
                        pixels[index] = TO_RGB565(255, 0, 0);
                        f32 height = heightmap_accessor_component->get_height(glm::vec3(offset.x + vertices_offset.x, 0.f, offset.z + vertices_offset.y)) + heightmap_constants::k_deep;
                        glm::vec3 normal = heightmap_accessor_component->get_normal(glm::vec3(offset.x + vertices_offset.x, 0.f, offset.z + vertices_offset.y));
                        
                        f32 normalized_height = height / max_height;
                        ui8 red = normalized_height <= heightmap_constants::k_layer_section_01 ? 255 : 0;
                        if(normalized_height > heightmap_constants::k_layer_section_01 &&
                           normalized_height <= heightmap_constants::k_layer_section_01 + heightmap_constants::k_layer_section_offset)
                        {
                            f32 interpolation = (normalized_height - heightmap_constants::k_layer_section_01) / heightmap_constants::k_layer_section_offset;
                            red = glm::mix(255, 0, interpolation);
                        }
                        
                        ui8 green = normalized_height > heightmap_constants::k_layer_section_01 && normalized_height <= heightmap_constants::k_layer_section_02 ? 255 : 0;
                        if(normalized_height < heightmap_constants::k_layer_section_01 &&
                           normalized_height >= heightmap_constants::k_layer_section_01 - heightmap_constants::k_layer_section_offset)
                        {
                            f32 interpolation = (normalized_height - (heightmap_constants::k_layer_section_01 - heightmap_constants::k_layer_section_offset)) /
                            heightmap_constants::k_layer_section_offset;
                            green = glm::mix(0, 255, interpolation);
                        }
                        else if(normalized_height > heightmap_constants::k_layer_section_02 &&
                                normalized_height <= heightmap_constants::k_layer_section_02 + heightmap_constants::k_layer_section_offset)
                        {
                            f32 interpolation = (normalized_height - heightmap_constants::k_layer_section_02) / heightmap_constants::k_layer_section_offset;
                            green = glm::mix(255, 0, interpolation);
                        }
                        
                        ui8 blue = normalized_height > heightmap_constants::k_layer_section_02 ? 255 : 0;
                        if(normalized_height < heightmap_constants::k_layer_section_02 &&
                           normalized_height >= heightmap_constants::k_layer_section_02 - heightmap_constants::k_layer_section_offset)
                        {
                            f32 interpolation = (normalized_height - (heightmap_constants::k_layer_section_02 - heightmap_constants::k_layer_section_offset)) /
                            heightmap_constants::k_layer_section_offset;
                            blue = glm::mix(0, 255, interpolation);
                        }
                        
                        f32 angle = glm::dot(glm::vec3(0.f, 1.f, 0.f), normal);
                        angle = glm::degrees(acosf(angle));
                        assert(angle >= 0.f);
                        angle = std::min(angle / 45.f, 1.f);
                        blue = std::max(static_cast<ui8>(glm::mix(0, 255, angle)), blue);
                        
                        pixels[index] = TO_RGB565(red, green, blue);
                        
                        offset.z += step.y;
                    }
                    offset.x += step.x;
                }
                
                if(stream && stream->is_open())
                {
                    stream->write((char*)&pixels[0], sizeof(ui16) * heightmap_constants::k_splatting_texture_mask_size.x * heightmap_constants::k_splatting_texture_mask_size.y);
                }
                else
                {
                    ui32 index = i + j * chunks_count.x;
                    assert(heightmap_container_component->get_splatting_mask_textures_mmap(index)->get_pointer());
                    memcpy(heightmap_container_component->get_splatting_mask_textures_mmap(index)->get_pointer(), pixels, sizeof(ui16) * heightmap_constants::k_splatting_texture_mask_size.x * heightmap_constants::k_splatting_texture_mask_size.y);
                }
                
                delete[] pixels;
                pixels = nullptr;
            }
        }
        stream->close();
    }
    
    void heightmap_textures_generator::create_splatting_diffuse_textures(const ces_entity_shared_ptr& entity,
                                                                         const std::function<void(i32 current, i32 total)>& callback)
    {
        const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
        const auto& heightmap_mmap = heightmap_container_component->get_mmap();
        glm::ivec2 chunks_count = heightmap_container_component->get_chunks_count();

        std::shared_ptr<std::ofstream> stream = std::make_shared<std::ofstream>();
        stream->open(heightmap_mmap::get_splatting_textures_diffuse_mmap_filename(heightmap_mmap->get_filename()), std::ios::binary | std::ios::out | std::ios::trunc);
        if(!stream->is_open())
        {
            assert(false);
        }
        
        const auto& heightmap_preprocessing_component = entity->get_component<ces_heightmap_preprocessing_component>();
        heightmap_preprocessing_component->set_splatting_processing_stream(stream);
        
        for(i32 i = 0; i < chunks_count.x; ++i)
        {
            for(i32 j = 0; j < chunks_count.y; ++j)
            {
                heightmap_textures_generator::create_splatting_texture(entity, i, j, callback);
            }
        }
    }
    
    void heightmap_textures_generator::generate_splatting_mask_textures(const ces_entity_shared_ptr& entity)
    {
        const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
        const auto& heightmap_mmap = heightmap_container_component->get_mmap();
        
        if(!heightmap_mmap::is_splatting_textures_mask_mmap_exist(heightmap_mmap->get_filename()))
        {
            heightmap_textures_generator::create_splatting_mask_textures(entity);
        }
    }
    
    void heightmap_textures_generator::generate_splatting_diffuse_textures(const ces_entity_shared_ptr& entity, const resource_accessor_shared_ptr& resource_accessor, const std::function<void(i32 current, i32 total)>& callback)
    {
        const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
        const auto& heightmap_mmap = heightmap_container_component->get_mmap();
        
        if(!heightmap_mmap::is_splatting_textures_diffuse_mmap_exist(heightmap_mmap->get_filename()))
        {
            const auto& heightmap_container_component = entity->get_component<ces_heightmap_container_component>();
            const auto& heightmap_configuration_component = entity->get_component<ces_heightmap_configuration_component>();
            const auto& heightmap_preprocessing_component = entity->get_component<ces_heightmap_preprocessing_component>();
            
            auto configuration_accessor = std::make_shared<gb::configuration_accessor>();
            auto material_configuration = configuration_accessor->get_material_configuration(heightmap_configuration_component->get_splatting_preprocessing_material_filename());
            auto material = gb::material::construct(std::static_pointer_cast<gb::material_configuration>(material_configuration));
            gb::material::set_shader(material, std::static_pointer_cast<gb::material_configuration>(material_configuration), resource_accessor, true);
            gb::material::set_textures(material, std::static_pointer_cast<gb::material_configuration>(material_configuration), resource_accessor, true);
            heightmap_preprocessing_component->set_splatting_preprocessing_material(material);
            
            glm::ivec2 splatting_mask_texture_size = glm::ivec2(sqrt(heightmap_container_component->get_splatting_mask_textures_mmap(0)->get_size()));
            ui32 splatting_mask_texture_id;
            gl_create_textures(1, &splatting_mask_texture_id);
            
            auto splatting_mask_texture = texture::construct("splatting.mask", splatting_mask_texture_id,
                                                             splatting_mask_texture_size.x, splatting_mask_texture_size.y);
            splatting_mask_texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
            splatting_mask_texture->set_mag_filter(GL_LINEAR);
            splatting_mask_texture->set_min_filter(GL_LINEAR);
            material->set_texture(splatting_mask_texture, e_shader_sampler_04);
            heightmap_preprocessing_component->set_splatting_mask_texture_buffer(splatting_mask_texture);
            
            for(i32 i = 0; i < heightmap_constants::e_heightmap_lod_max; ++i)
            {
                glm::ivec2 texture_size = heightmap_container_component->get_textures_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(i));
                heightmap_preprocessing_component->set_render_target(std::make_shared<render_target>(GL_RGBA, glm::ivec2(texture_size.x, texture_size.y)), static_cast<heightmap_constants::e_heightmap_lod>(i));
            }
            heightmap_textures_generator::create_splatting_diffuse_textures(entity, callback);
        }
        else
        {
            callback(0, 0);
        }
    }
}
