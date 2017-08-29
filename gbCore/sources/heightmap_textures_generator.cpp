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
#include "heightmap_mmap.h"
#include "heightmap_constants.h"
#include "ces_entity.h"

namespace gb
{
    void heightmap_textures_generator::create_splatting_texture(const resource_accessor_shared_ptr& resource_accessor,
                                                                const ces_entity_shared_ptr& entity,
                                                                i32 i, i32 j, const std::function<void(ui8 *, ui32, i32, i32, i32)>& callback)
    {
        
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
                
                glm::vec3 offset = glm::vec3(0.0f);
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
                        assert(angle >= 0.0);
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
}
