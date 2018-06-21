//
//  ces_heightmap_container_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_heightmap_container_component.h"
#include "common.h"

namespace gb
{
    ces_heightmap_container_component::ces_heightmap_container_component() :
    m_heightmap_mmap(nullptr),
    m_is_generated(false),
    m_is_generating(false)
    {
        
    }
    
    ces_heightmap_container_component::~ces_heightmap_container_component()
    {

    }
    
    void ces_heightmap_container_component::setup(const std::shared_ptr<heightmap_mmap>& heightmap_mmap, const glm::ivec2& chunk_size)
    {
        m_heightmap_mmap = heightmap_mmap;
        glm::ivec2 heightmap_size = m_heightmap_mmap->get_heightmap_size();
        
        m_chunk_size = glm::ivec2(std::min(heightmap_size.x, chunk_size.x),
                                  std::min(heightmap_size.y, chunk_size.y));
        
        m_chunk_lods_sizes.at(heightmap_constants::e_heightmap_lod_01) = glm::ivec2(std::min(heightmap_size.x, m_chunk_size.x),
                                                               std::min(heightmap_size.y, m_chunk_size.y));
        
        m_chunk_lods_sizes.at(heightmap_constants::e_heightmap_lod_02) = glm::ivec2(std::min(heightmap_size.x, static_cast<i32>(floor(static_cast<f32>(m_chunk_size.x) / 2.f))),
                                                               std::min(heightmap_size.y, static_cast<i32>(floor(static_cast<f32>(m_chunk_size.y) / 2.f))));
        
        m_chunk_lods_sizes.at(heightmap_constants::e_heightmap_lod_03) = glm::ivec2(std::min(heightmap_size.x, static_cast<i32>(ceil(static_cast<f32>(m_chunk_size.x) / 4.f))),
                                                               std::min(heightmap_size.y, static_cast<i32>(ceil(static_cast<f32>(m_chunk_size.y) / 4.f))));
        
        m_chunk_lods_sizes.at(heightmap_constants::e_heightmap_lod_04) = glm::ivec2(std::min(heightmap_size.x, static_cast<i32>(floor(static_cast<f32>(m_chunk_size.x) / 8.f))),
                                                               std::min(heightmap_size.y, static_cast<i32>(floor(static_cast<f32>(m_chunk_size.y) / 8.f))));
        
        m_textures_lods_sizes.at(heightmap_constants::e_heightmap_lod_01) = heightmap_constants::k_splatting_texture_size[heightmap_constants::e_heightmap_lod_01];
        m_textures_lods_sizes.at(heightmap_constants::e_heightmap_lod_02) = heightmap_constants::k_splatting_texture_size[heightmap_constants::e_heightmap_lod_02];
        m_textures_lods_sizes.at(heightmap_constants::e_heightmap_lod_03) = heightmap_constants::k_splatting_texture_size[heightmap_constants::e_heightmap_lod_03];
        m_textures_lods_sizes.at(heightmap_constants::e_heightmap_lod_04) = heightmap_constants::k_splatting_texture_size[heightmap_constants::e_heightmap_lod_04];
        
        m_chunks_count = glm::ivec2(heightmap_size.x / (m_chunk_size.x - 1),
                                    heightmap_size.y / (m_chunk_size.y - 1));
        
        m_vbos_mmap.resize(m_chunks_count.x * m_chunks_count.y);
        m_ibos_mmap.resize(m_chunks_count.x * m_chunks_count.y);
        
        m_splatting_mask_textures_mmap.resize(m_chunks_count.x * m_chunks_count.y);
        m_splatting_diffuse_textures_mmap.resize(m_chunks_count.x * m_chunks_count.y);
        m_splatting_normal_textures_mmap.resize(m_chunks_count.x * m_chunks_count.y);
    }
    
    std::shared_ptr<heightmap_mmap> ces_heightmap_container_component::get_mmap() const
    {
        return m_heightmap_mmap;
    }
    
    void ces_heightmap_container_component::mmap_geometry(const std::string& filename)
    {
        m_heightmap_mmap->set_compressed_vertices(static_cast<heightmap_mmap::compressed_vertex*>(m_heightmap_mmap->get_compressed_vertices_mmap_descriptor()->allocate(heightmap_mmap::get_compressed_vertices_mmap_filename(filename))));
        
        m_heightmap_mmap->set_uncompressed_vertices(static_cast<heightmap_mmap::uncomressed_vertex*>(m_heightmap_mmap->get_uncompressed_vertices_mmap_descriptor()->allocate(heightmap_mmap::get_uncompressed_vertices_mmap_filename(filename))));
        
        m_heightmap_mmap->set_faces(static_cast<heightmap_mmap::face*>(m_heightmap_mmap->get_faces_mmap_descriptor()->allocate(heightmap_mmap::get_faces_mmap_filename(filename))));
        
        m_heightmap_mmap->get_vbos_mmap_descriptor()->allocate(heightmap_mmap::get_vbos_mmap_filename(filename));
        
        i32 offset = 0;
        for(i32 i = 0; i < m_chunks_count.x; ++i)
        {
            for(i32 j = 0; j < m_chunks_count.y; ++j)
            {
                m_vbos_mmap[i + j * m_chunks_count.x] = std::make_shared<heightmap_mmap::mmap_vbo>(m_heightmap_mmap->get_vbos_mmap_descriptor());
                m_vbos_mmap[i + j * m_chunks_count.x]->set_size(m_chunk_size.x * m_chunk_size.y);
                m_vbos_mmap[i + j * m_chunks_count.x]->set_offset(offset);
                offset += m_chunk_size.x * m_chunk_size.y;
            }
        }
        
        m_heightmap_mmap->get_ibos_mmap_descriptor()->allocate(heightmap_mmap::get_ibos_mmap_filename(filename));
        
        offset = 0;
        for(i32 i = 0; i < m_chunks_count.x; ++i)
        {
            for(i32 j = 0; j < m_chunks_count.y; ++j)
            {
                for(i32 k = 0; k < heightmap_constants::e_heightmap_lod_max; ++k)
                {
                    glm::ivec2 current_chunk_size = glm::ivec2(m_chunk_lods_sizes[k].x % 2 == 0 ? m_chunk_lods_sizes[k].x : m_chunk_lods_sizes[k].x - 1,
                                                               m_chunk_lods_sizes[k].y % 2 == 0 ? m_chunk_lods_sizes[k].y : m_chunk_lods_sizes[k].y - 1);
                    
                    glm::ivec2 extended_chunk_size = current_chunk_size;
                    extended_chunk_size -= glm::ivec2(k != heightmap_constants::e_heightmap_lod_01 ? 2 : 0,
                                                      k != heightmap_constants::e_heightmap_lod_01 ? 2 : 0);
                    
                    ui32 additional_indices_count = k != heightmap_constants::e_heightmap_lod_01 ? 12 * (m_chunk_size.x - 1 + current_chunk_size.x) : 0;
                    ui32 indices_count = extended_chunk_size.x * extended_chunk_size.y * 6 + additional_indices_count;
                    
                    m_ibos_mmap[i + j * m_chunks_count.x][k] = std::make_shared<heightmap_mmap::mmap_ibo>(m_heightmap_mmap->get_ibos_mmap_descriptor());
                    m_ibos_mmap[i + j * m_chunks_count.x][k]->set_size(indices_count);
                    m_ibos_mmap[i + j * m_chunks_count.x][k]->set_offset(offset);
                    offset += indices_count * 2;
                }
            }
        }
    }
    
    void ces_heightmap_container_component::mmap_mask_textures(const std::string& filename)
    {
        m_heightmap_mmap->get_splatting_mask_textures_mmap_descriptor()->allocate(heightmap_mmap::get_splatting_textures_mask_mmap_filename(filename));
        
        i32 offset = 0;
        for(i32 i = 0; i < m_chunks_count.x; ++i)
        {
            for(i32 j = 0; j < m_chunks_count.y; ++j)
            {
                m_splatting_mask_textures_mmap[i + j * m_chunks_count.x] = std::make_shared<heightmap_mmap::mmap_RGB565>(m_heightmap_mmap->get_splatting_mask_textures_mmap_descriptor());
                m_splatting_mask_textures_mmap[i + j * m_chunks_count.x]->set_size(heightmap_constants::k_splatting_texture_mask_size.x * heightmap_constants::k_splatting_texture_mask_size.y);
                m_splatting_mask_textures_mmap[i + j * m_chunks_count.x]->set_offset(offset);
                offset += heightmap_constants::k_splatting_texture_mask_size.x * heightmap_constants::k_splatting_texture_mask_size.y;
            }
        }
    }
    
    void ces_heightmap_container_component::mmap_diffuse_textures(const std::string& filename)
    {
        m_heightmap_mmap->get_splatting_diffuse_textures_mmap_descriptor()->allocate(heightmap_mmap::get_splatting_textures_diffuse_mmap_filename(filename));
        
        i32 offset = 0;
        for(i32 i = 0; i < m_chunks_count.x; ++i)
        {
            for(i32 j = 0; j < m_chunks_count.y; ++j)
            {
                for(i32 k = 0; k < heightmap_constants::e_heightmap_lod_max; ++k)
                {
                    m_splatting_diffuse_textures_mmap[i + j * m_chunks_count.x][k] = std::make_shared<heightmap_mmap::mmap_RGB565>(m_heightmap_mmap->get_splatting_diffuse_textures_mmap_descriptor());
                    m_splatting_diffuse_textures_mmap[i + j * m_chunks_count.x][k]->set_size(ces_heightmap_container_component::get_textures_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).x *
                                                                                             ces_heightmap_container_component::get_textures_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).y);
                    m_splatting_diffuse_textures_mmap[i + j * m_chunks_count.x][k]->set_offset(offset);
                    offset += ces_heightmap_container_component::get_textures_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).x *
                    ces_heightmap_container_component::get_textures_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).y;
                }
            }
        }
    }
    
    void ces_heightmap_container_component::mmap_normal_textures(const std::string& filename)
    {
        m_heightmap_mmap->get_splatting_normal_textures_mmap_descriptor()->allocate(heightmap_mmap::get_splatting_textures_normal_mmap_filename(filename));
        
        i32 offset = 0;
        for(i32 i = 0; i < m_chunks_count.x; ++i)
        {
            for(i32 j = 0; j < m_chunks_count.y; ++j)
            {
                for(i32 k = 0; k < heightmap_constants::e_heightmap_lod_max; ++k)
                {
                    m_splatting_normal_textures_mmap[i + j * m_chunks_count.x][k] = std::make_shared<heightmap_mmap::mmap_RGBA8>(m_heightmap_mmap->get_splatting_normal_textures_mmap_descriptor());
                    m_splatting_normal_textures_mmap[i + j * m_chunks_count.x][k]->set_size(ces_heightmap_container_component::get_textures_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).x *
                                                                                            ces_heightmap_container_component::get_textures_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).y *
                                                                                            heightmap_constants::k_splatting_texture_channels);
                    m_splatting_normal_textures_mmap[i + j * m_chunks_count.x][k]->set_offset(offset);
                    offset += ces_heightmap_container_component::get_textures_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).x *
                    ces_heightmap_container_component::get_textures_lod_size(static_cast<heightmap_constants::e_heightmap_lod>(k)).y * heightmap_constants::k_splatting_texture_channels;
                }
            }
        }
    }
    
    bool ces_heightmap_container_component::get_is_generated() const
    {
        return m_is_generated;
    }
    
    bool ces_heightmap_container_component::get_is_generating() const
    {
        return m_is_generating;
    }
    
    void ces_heightmap_container_component::set_is_generated(bool value)
    {
        m_is_generated = value;
    }
    
    void ces_heightmap_container_component::set_is_generating(bool value)
    {
        m_is_generating = value;
    }
    
    glm::ivec2 ces_heightmap_container_component::get_chunks_count() const
    {
        return m_chunks_count;
    };
    
    glm::ivec2 ces_heightmap_container_component::get_chunk_size() const
    {
        return m_chunk_size;
    };
    
    glm::ivec2 ces_heightmap_container_component::get_chunk_lod_size(heightmap_constants::e_heightmap_lod lod) const
    {
        assert(lod >= 0 && lod < heightmap_constants::e_heightmap_lod_max);
        return m_chunk_lods_sizes[lod];
    }
    
    glm::ivec2 ces_heightmap_container_component::get_textures_lod_size(heightmap_constants::e_heightmap_lod lod) const
    {
        assert(lod >= 0 && lod < heightmap_constants::e_heightmap_lod_max);
        return m_textures_lods_sizes[lod];
    }
    
    f32 ces_heightmap_container_component::get_max_height() const
    {
        auto vertices = m_heightmap_mmap->get_compressed_vertices();
        auto heightmap_size = m_heightmap_mmap->get_heightmap_size();
        auto max = std::max_element(vertices, vertices + heightmap_size.x * heightmap_size.y, [](heightmap_mmap::compressed_vertex const& value_01,
                                                                                                 heightmap_mmap::compressed_vertex const& value_02) {
            return value_01.m_position.y > value_02.m_position.y;
        });
        return max->m_position.y;
    }
    
    f32 ces_heightmap_container_component::get_min_height() const
    {
        auto vertices = m_heightmap_mmap->get_compressed_vertices();
        auto heightmap_size = m_heightmap_mmap->get_heightmap_size();
        auto min = std::max_element(vertices, vertices + heightmap_size.x * heightmap_size.y, [] (heightmap_mmap::compressed_vertex const& value_01,
                                                                                                  heightmap_mmap::compressed_vertex const& value_02) {
            return value_01.m_position.y < value_02.m_position.y;
        });
        return min->m_position.y;
    }
    
    std::shared_ptr<heightmap_mmap::mmap_vbo> ces_heightmap_container_component::get_vbo_mmap(i32 index) const
    {
        return m_vbos_mmap.at(index);
    }
    
    std::shared_ptr<heightmap_mmap::mmap_ibo> ces_heightmap_container_component::get_ibo_mmap(i32 index, heightmap_constants::e_heightmap_lod lod) const
    {
        return m_ibos_mmap.at(index).at(lod);
    }
    
    std::shared_ptr<heightmap_mmap::mmap_RGB565> ces_heightmap_container_component::get_splatting_mask_textures_mmap(i32 index) const
    {
        return m_splatting_mask_textures_mmap.at(index);
    }
    
    std::shared_ptr<heightmap_mmap::mmap_RGB565> ces_heightmap_container_component::get_splatting_diffuse_textures_mmap(i32 index, heightmap_constants::e_heightmap_lod lod) const
    {
        return m_splatting_diffuse_textures_mmap.at(index).at(lod);
    }
    
    std::shared_ptr<heightmap_mmap::mmap_RGBA8> ces_heightmap_container_component::get_splatting_normal_textures_mmap(i32 index, heightmap_constants::e_heightmap_lod lod) const
    {
        return m_splatting_normal_textures_mmap.at(index).at(lod);
    }
}
