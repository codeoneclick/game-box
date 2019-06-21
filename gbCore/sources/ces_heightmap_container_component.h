//
//  ces_heightmap_container_component.h
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "memory_map.h"
#include "vbo.h"
#include "heightmap_constants.h"
#include "heightmap_mmap.h"

namespace gb
{
    class ces_heightmap_container_component : public ces_base_component
    {
    private:
        
        bool m_is_generated;
        bool m_is_generating;
        std::shared_ptr<heightmap_mmap> m_heightmap_mmap;
        
        std::vector<std::shared_ptr<heightmap_mmap::mmap_vbo>> m_vbos_mmap;
        std::vector<std::array<std::shared_ptr<heightmap_mmap::mmap_ibo>, heightmap_constants::e_heightmap_lod_max>> m_ibos_mmap;
        
        std::vector<std::shared_ptr<heightmap_mmap::mmap_RGB565>> m_splatting_mask_textures_mmap;
        std::vector<std::array<std::shared_ptr<heightmap_mmap::mmap_RGB565>, heightmap_constants::e_heightmap_lod_max>> m_splatting_diffuse_textures_mmap;
        std::vector<std::array<std::shared_ptr<heightmap_mmap::mmap_RGBA8>, heightmap_constants::e_heightmap_lod_max>> m_splatting_normal_textures_mmap;
        
        glm::ivec2 m_chunks_count;
        glm::ivec2 m_chunk_size;
        std::array<glm::ivec2, heightmap_constants::e_heightmap_lod_max> m_chunk_lods_sizes;
        std::array<glm::ivec2, heightmap_constants::e_heightmap_lod_max> m_textures_lods_sizes;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_heightmap_container_component, ces_base_component::g_guids_container)
        ces_heightmap_container_component();
        ~ces_heightmap_container_component();
        
        void setup(const std::shared_ptr<heightmap_mmap>& heightmap_mmap, const glm::ivec2& chunk_size);
        
        std::shared_ptr<heightmap_mmap> get_mmap() const;
        
        void mmap_geometry(const std::string& filename);
        void mmap_mask_textures(const std::string& filename);
        void mmap_diffuse_textures(const std::string& filename);
        void mmap_normal_textures(const std::string& filename);
        
        glm::ivec2 get_chunks_count() const;
        glm::ivec2 get_chunk_size() const;
        glm::ivec2 get_chunk_lod_size(heightmap_constants::e_heightmap_lod lod) const;
        glm::ivec2 get_textures_lod_size(heightmap_constants::e_heightmap_lod lod) const;
        
        f32 get_max_height() const;
        f32 get_min_height() const;
        
        std::shared_ptr<heightmap_mmap::mmap_vbo> get_vbo_mmap(i32 index) const;
        std::shared_ptr<heightmap_mmap::mmap_ibo> get_ibo_mmap(i32 index, heightmap_constants::e_heightmap_lod lod) const;
        std::shared_ptr<heightmap_mmap::mmap_RGB565> get_splatting_mask_textures_mmap(i32 index) const;
        std::shared_ptr<heightmap_mmap::mmap_RGB565> get_splatting_diffuse_textures_mmap(i32 index, heightmap_constants::e_heightmap_lod lod) const;
        std::shared_ptr<heightmap_mmap::mmap_RGBA8> get_splatting_normal_textures_mmap(i32 index, heightmap_constants::e_heightmap_lod lod) const;
        
        bool get_is_generated() const;
        bool get_is_generating() const;
        
        void set_is_generated(bool value);
        void set_is_generating(bool value);
    };
};
