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
        
        CTTI_CLASS_GUID(ces_heightmap_container_component, ces_base_component::g_guids_container)
        ces_heightmap_container_component();
        ~ces_heightmap_container_component();
        
        void setup(const std::shared_ptr<heightmap_mmap>& heightmap_mmap, const glm::ivec2& chunk_size);
        
        void mmap_geometry(const std::string& filename);
        void mmap_mask_textures(const std::string& filename);
        void mmap_diffuse_textures(const std::string& filename);
        void mmap_normal_textures(const std::string& filename);
        
        inline glm::ivec2 get_heightmap_size() const;
        inline glm::ivec2 get_chunks_count() const;
        inline glm::ivec2 get_chunk_size() const;
        inline glm::ivec2 get_chunk_lod_size(heightmap_constants::e_heightmap_lod lod) const;
        inline glm::ivec2 get_textures_lod_size(heightmap_constants::e_heightmap_lod lod) const;
        
        inline f32 get_max_height() const;
        inline f32 get_min_height() const;
        
        inline void attach_uncompressed_vertex_to_vbo(i32 i, i32 j, ui32 vbo_index, ui32 vbo_vertex_index);
        inline glm::ivec2* attached_vertices_to_vbo(i32 i, i32 j, ui8 *size) const;
        
        inline void attach_uncompressed_vertex_to_face(i32 i, i32 j, ui32 face_index);
        inline ui32* attached_vertices_to_face(i32 i, i32 j, ui8 *size) const;
        
        inline glm::vec3 get_vertex_position(ui32 i, ui32 j) const;
        inline glm::uint32 get_compressed_vertex_texcoord(ui32 i, ui32 j) const;
        inline glm::vec2 get_uncompressed_vertex_texcoord(ui32 i, ui32 j) const;
        inline glm::uint32 get_compressed_vertex_normal(ui32 i, ui32 j) const;
        inline glm::vec3 get_uncompressed_vertex_normal(ui32 i, ui32 j) const;
        
        inline std::shared_ptr<heightmap_mmap::mmap_vbo> get_vbo_mmap(i32 index) const;
        inline std::shared_ptr<heightmap_mmap::mmap_ibo> get_ibo_mmap(i32 index, heightmap_constants::e_heightmap_lod lod) const;
        inline std::shared_ptr<heightmap_mmap::mmap_RGB565> get_splatting_mask_textures_mmap(i32 index) const;
        inline std::shared_ptr<heightmap_mmap::mmap_RGB565> get_splatting_diffuse_textures_mmap(i32 index, heightmap_constants::e_heightmap_lod lod) const;
        inline std::shared_ptr<heightmap_mmap::mmap_RGBA8> get_splatting_noraml_textures_mmap(i32 index, heightmap_constants::e_heightmap_lod lod) const;
    };
};
