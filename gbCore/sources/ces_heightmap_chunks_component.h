//
//  ces_heightmap_chunks_component.h
//  gbCore
//
//  Created by serhii serhiiv on 8/30/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "heightmap_constants.h"

namespace gb
{
    class ces_heightmap_chunks_component : public ces_base_component
    {
    public:
        
        typedef std::function<void(const mesh_3d_shared_ptr&)> heightmap_chunk_mesh_loaded_t;
        typedef std::function<void(const quad_tree_shared_ptr&)> heightmap_chunk_quad_tree_loaded_t;
        typedef std::function<void(const texture_shared_ptr&, const texture_shared_ptr&)> heightmap_chunk_textures_loaded_t;
        
    private:
        
        std::vector<ces_entity_shared_ptr> m_chunks;
        std::vector<thread_operation_shared_ptr> m_executed_operations;
        std::vector<std::tuple<heightmap_chunk_mesh_loaded_t, heightmap_chunk_quad_tree_loaded_t, heightmap_chunk_textures_loaded_t>> m_callbacks;
        std::vector<std::tuple<mesh_3d_shared_ptr, quad_tree_shared_ptr, texture_shared_ptr, texture_shared_ptr, heightmap_constants::e_heightmap_lod, std::shared_ptr<std::mutex>>> m_chunks_metadata;
        
        std::array<std::queue<texture_shared_ptr>, heightmap_constants::e_heightmap_lod_max> m_splatting_diffuse_textures_cache;
        std::array<std::queue<texture_shared_ptr>, heightmap_constants::e_heightmap_lod_max> m_splatting_normal_textures_cache;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_heightmap_chunks_component, ces_base_component::g_guids_container)
        ces_heightmap_chunks_component();
        ~ces_heightmap_chunks_component();
        
        void setup(const glm::ivec2& chunks_count);
        
        std::vector<ces_entity_shared_ptr>& get_chunks();
        std::vector<thread_operation_shared_ptr>& get_executed_operations();
        std::vector<std::tuple<heightmap_chunk_mesh_loaded_t, heightmap_chunk_quad_tree_loaded_t, heightmap_chunk_textures_loaded_t>>& get_callbacks();
        std::vector<std::tuple<mesh_3d_shared_ptr, quad_tree_shared_ptr, texture_shared_ptr, texture_shared_ptr, heightmap_constants::e_heightmap_lod, std::shared_ptr<std::mutex>>>& get_chunks_metadata();
        
        heightmap_constants::e_heightmap_lod get_chunk_lod(const glm::vec3& point,
                                                           const glm::vec3& min_bound,
                                                           const glm::vec3& max_bound) const;
        
        std::array<std::queue<texture_shared_ptr>, heightmap_constants::e_heightmap_lod_max>& get_splatting_diffuse_textures_cache();
        std::array<std::queue<texture_shared_ptr>, heightmap_constants::e_heightmap_lod_max>& get_splatting_normal_textures_cache();
    };
};

