//
//  ces_heightmap_chunks_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/30/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_heightmap_chunks_component.h"

namespace gb
{
    ces_heightmap_chunks_component::ces_heightmap_chunks_component()
    {
        
    }
    
    ces_heightmap_chunks_component::~ces_heightmap_chunks_component()
    {
        
    }
    
    void ces_heightmap_chunks_component::setup(const glm::ivec2& chunks_count)
    {
        m_chunks.resize(chunks_count.x * chunks_count.y, nullptr);
        m_callbacks.resize(chunks_count.x * chunks_count.y,
                           std::make_tuple(nullptr, nullptr, nullptr));
        m_executed_operations.resize(chunks_count.x * chunks_count.y, nullptr);
        m_chunks_metadata.resize(chunks_count.x * chunks_count.y,
                                 std::make_tuple(nullptr, nullptr, nullptr, nullptr, heightmap_constants::e_heightmap_lod_unknown, std::make_shared<std::mutex>()));
    }
    
    std::vector<ces_entity_shared_ptr>& ces_heightmap_chunks_component::get_chunks()
    {
        return m_chunks;
    }
    
    std::vector<thread_operation_shared_ptr>& ces_heightmap_chunks_component::get_executed_operations()
    {
        return m_executed_operations;
    }
    
    std::vector<std::tuple< ces_heightmap_chunks_component::heightmap_chunk_mesh_loaded_t,
    ces_heightmap_chunks_component::heightmap_chunk_quad_tree_loaded_t,
    ces_heightmap_chunks_component::heightmap_chunk_textures_loaded_t>>& ces_heightmap_chunks_component::get_callbacks()
    {
        return m_callbacks;
    }
    
    std::vector<std::tuple<mesh_3d_shared_ptr, quad_tree_shared_ptr, texture_shared_ptr, texture_shared_ptr, heightmap_constants::e_heightmap_lod, std::shared_ptr<std::mutex>>>& ces_heightmap_chunks_component::get_chunks_metadata()
    {
        return m_chunks_metadata;
    }
    
    std::array<std::queue<texture_shared_ptr>, heightmap_constants::e_heightmap_lod_max>& ces_heightmap_chunks_component::get_splatting_diffuse_textures_cache()
    {
        return m_splatting_diffuse_textures_cache;
    }
    
    std::array<std::queue<texture_shared_ptr>, heightmap_constants::e_heightmap_lod_max>& ces_heightmap_chunks_component::get_splatting_normal_textures_cache()
    {
        return m_splatting_normal_textures_cache;
    }
    
    heightmap_constants::e_heightmap_lod ces_heightmap_chunks_component::get_chunk_lod(const glm::vec3& point,
                                                                                       const glm::vec3& min_bound,
                                                                                       const glm::vec3& max_bound) const
    {
        glm::vec2 center = glm::vec2((max_bound.x - min_bound.x) / 2.f + min_bound.x,
                                     (max_bound.z - min_bound.z) / 2.f + min_bound.z);
        f32 distance = glm::distance(glm::vec2(point.x, point.z), center);
        heightmap_constants::e_heightmap_lod lod = heightmap_constants::e_heightmap_lod_04;
        if(distance < heightmap_constants::k_lod_01_distance)
        {
            lod = heightmap_constants::e_heightmap_lod_01;
        }
        else if(distance < heightmap_constants::k_lod_02_distance)
        {
            lod = heightmap_constants::e_heightmap_lod_02;
        }
        else if(distance < heightmap_constants::k_lod_03_distance)
        {
            lod = heightmap_constants::e_heightmap_lod_03;
        }
        return lod;
    }
}
