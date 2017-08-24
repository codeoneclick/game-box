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
    static const std::string k_uncompressed_vertices_data_filename = "-uncompressed.vertices.data";
    static const std::string k_compressed_vertices_data_filename = "-compressed.vertices.data";
    static const std::string k_faces_data_filename = "-faces.data";
    static const std::string k_vbos_data_filename = "-vbos.data";
    static const std::string k_ibos_data_filename = "-ibos.data";
    static const std::string k_splatting_textures_mask_data_filename = "-splatting.textures.m.data";
    static const std::string k_splatting_textures_diffuse_data_filename = "-splatting.textures.d.data";
    static const std::string k_splatting_textures_normal_data_filename = "-splatting.textures.n.data";
    static const std::string k_tangent_space_data_filename = "-tangent.space.data";
    static const std::string k_attaches_to_vbo_data_filename = "-attaches.vbo.data";
    
    ces_heightmap_container_component::ces_heightmap_container_component() :
    m_uncompressed_vertices_mmap_descriptor(nullptr),
    m_compressed_vertices_mmap_descriptor(nullptr),
    m_faces_mmap_descriptor(nullptr),
    m_vbos_mmap_descriptor(nullptr),
    m_ibos_mmap_descriptor(nullptr),
    m_splatting_mask_textures_mmap_descriptor(nullptr),
    m_splatting_diffuse_textures_mmap_descriptor(nullptr),
    m_splatting_normal_textures_mmap_descriptor(nullptr),
    m_heightmap_size(0)
    {
        
    }
    
    ces_heightmap_container_component::~ces_heightmap_container_component()
    {
        ces_heightmap_container_component::erase_geometry();
        ces_heightmap_container_component::erase_mask_textures();
        ces_heightmap_container_component::erase_diffuse_textures();
        ces_heightmap_container_component::erase_normal_textures();
    }
    
    std::string ces_heightmap_container_component::get_uncompressed_vertices_mmap_filename(const std::string &filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_uncompressed_vertices_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string ces_heightmap_container_component::get_compressed_vertices_mmap_filename(const std::string& filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_compressed_vertices_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string ces_heightmap_container_component::get_faces_mmap_filename(const std::string& filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_faces_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string ces_heightmap_container_component::get_vbos_mmap_filename(const std::string &filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_vbos_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string ces_heightmap_container_component::get_ibos_mmap_filename(const std::string &filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_ibos_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string ces_heightmap_container_component::get_splatting_textures_mask_mmap_filename(const std::string &filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_splatting_textures_mask_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string ces_heightmap_container_component::get_splatting_textures_normal_mmap_filename(const std::string& filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_splatting_textures_normal_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string ces_heightmap_container_component::get_splatting_textures_diffuse_mmap_filename(const std::string& filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_splatting_textures_diffuse_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string ces_heightmap_container_component::get_tangent_space_mmap_filename(const std::string& filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_tangent_space_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string ces_heightmap_container_component::get_attaches_to_vbo_mmap_filename(const std::string &filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_attaches_to_vbo_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    bool ces_heightmap_container_component::is_uncompressed_vertices_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(ces_heightmap_container_component::get_uncompressed_vertices_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool ces_heightmap_container_component::is_compressed_vertices_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(ces_heightmap_container_component::get_compressed_vertices_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool ces_heightmap_container_component::is_faces_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(ces_heightmap_container_component::get_faces_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool ces_heightmap_container_component::is_vbos_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(ces_heightmap_container_component::get_vbos_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool ces_heightmap_container_component::is_ibos_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(ces_heightmap_container_component::get_ibos_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool ces_heightmap_container_component::is_splatting_textures_mask_mmap_exist(const std::string &filename)
    {
        std::ifstream stream(ces_heightmap_container_component::get_splatting_textures_mask_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool ces_heightmap_container_component::is_splatting_textures_normal_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(ces_heightmap_container_component::get_splatting_textures_normal_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool ces_heightmap_container_component::is_splatting_textures_diffuse_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(ces_heightmap_container_component::get_splatting_textures_diffuse_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool ces_heightmap_container_component::is_tangent_space_mmap_exist(const std::string &filename)
    {
        std::ifstream stream(ces_heightmap_container_component::get_tangent_space_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool ces_heightmap_container_component::is_attaches_to_vbo_mmap_exist(const std::string &filename)
    {
        std::ifstream stream(ces_heightmap_container_component::get_attaches_to_vbo_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    void ces_heightmap_container_component::init(const glm::ivec2& heightmap_size, const glm::ivec2& chunk_size)
    {
        m_heightmap_size = heightmap_size;
        
        m_chunk_size = glm::ivec2(std::min(m_heightmap_size.x, chunk_size.x),
                                  std::min(m_heightmap_size.y, chunk_size.y));
        
        m_chunk_lods_sizes.at(heightmap_constants::e_heightmap_lod_01) = glm::ivec2(std::min(m_heightmap_size.x, m_chunk_size.x),
                                                               std::min(m_heightmap_size.y, m_chunk_size.y));
        
        m_chunk_lods_sizes.at(heightmap_constants::e_heightmap_lod_02) = glm::ivec2(std::min(m_heightmap_size.x, static_cast<i32>(floor(static_cast<f32>(m_chunk_size.x) / 2.f))),
                                                               std::min(m_heightmap_size.y, static_cast<i32>(floor(static_cast<f32>(m_chunk_size.y) / 2.f))));
        
        m_chunk_lods_sizes.at(heightmap_constants::e_heightmap_lod_03) = glm::ivec2(std::min(m_heightmap_size.x, static_cast<i32>(ceil(static_cast<f32>(m_chunk_size.x) / 4.f))),
                                                               std::min(m_heightmap_size.y, static_cast<i32>(ceil(static_cast<f32>(m_chunk_size.y) / 4.f))));
        
        m_chunk_lods_sizes.at(heightmap_constants::e_heightmap_lod_04) = glm::ivec2(std::min(m_heightmap_size.x, static_cast<i32>(floor(static_cast<f32>(m_chunk_size.x) / 8.f))),
                                                               std::min(m_heightmap_size.y, static_cast<i32>(floor(static_cast<f32>(m_chunk_size.y) / 8.f))));
        
        m_textures_lods_sizes.at(heightmap_constants::e_heightmap_lod_01) = heightmap_constants::k_splatting_texture_size[heightmap_constants::e_heightmap_lod_01];
        m_textures_lods_sizes.at(heightmap_constants::e_heightmap_lod_02) = heightmap_constants::k_splatting_texture_size[heightmap_constants::e_heightmap_lod_02];
        m_textures_lods_sizes.at(heightmap_constants::e_heightmap_lod_03) = heightmap_constants::k_splatting_texture_size[heightmap_constants::e_heightmap_lod_03];
        m_textures_lods_sizes.at(heightmap_constants::e_heightmap_lod_04) = heightmap_constants::k_splatting_texture_size[heightmap_constants::e_heightmap_lod_04];
        
        m_chunks_count = glm::ivec2(m_chunk_size.x / (m_heightmap_size.x - 1),
                                    m_chunk_size.y / (m_heightmap_size.y - 1));
        
        m_vbos_mmap.resize(m_chunks_count.x * m_chunks_count.y);
        m_ibos_mmap.resize(m_chunks_count.x * m_chunks_count.y);
        
        m_splatting_mask_textures_mmap.resize(m_chunks_count.x * m_chunks_count.y);
        m_splatting_diffuse_textures_mmap.resize(m_chunks_count.x * m_chunks_count.y);
        m_splatting_normal_textures_mmap.resize(m_chunks_count.x * m_chunks_count.y);
    }
    
    void ces_heightmap_container_component::erase_geometry()
    {
        if(m_uncompressed_vertices_mmap_descriptor != 0)
        {
            m_uncompressed_vertices_mmap_descriptor->deallocate();
            m_uncompressed_vertices_mmap_descriptor = nullptr;
        }
        else
        {
            delete [] m_uncompressed_vertices;
        }
        m_uncompressed_vertices = nullptr;
        
        if(m_compressed_vertices_mmap_descriptor != nullptr)
        {
            m_compressed_vertices_mmap_descriptor->deallocate();
            m_compressed_vertices_mmap_descriptor = nullptr;
        }
        else
        {
            delete [] m_compressed_vertices;
        }
        m_compressed_vertices = nullptr;
        
        if(m_faces_mmap_descriptor != nullptr)
        {
            m_faces_mmap_descriptor->deallocate();
            m_faces_mmap_descriptor = nullptr;
        }
        else
        {
            delete [] m_faces;
        }
        m_faces = nullptr;
        
        if(m_vbos_mmap_descriptor != nullptr)
        {
            m_vbos_mmap_descriptor->deallocate();
            m_vbos_mmap_descriptor = nullptr;
        }
        
        if(m_ibos_mmap_descriptor != nullptr)
        {
            m_ibos_mmap_descriptor->deallocate();
            m_ibos_mmap_descriptor = nullptr;
        }
    }
    
    void ces_heightmap_container_component::erase_mask_textures()
    {
        if(m_splatting_mask_textures_mmap_descriptor)
        {
            m_splatting_mask_textures_mmap_descriptor->deallocate();
            m_splatting_mask_textures_mmap_descriptor = nullptr;
        }
    }
    
    void ces_heightmap_container_component::erase_diffuse_textures()
    {
        if(m_splatting_diffuse_textures_mmap_descriptor)
        {
            m_splatting_diffuse_textures_mmap_descriptor->deallocate();
            m_splatting_diffuse_textures_mmap_descriptor = nullptr;
        }
    }
    
    void ces_heightmap_container_component::erase_normal_textures()
    {
        if(m_splatting_normal_textures_mmap_descriptor)
        {
            m_splatting_normal_textures_mmap_descriptor->deallocate();
            m_splatting_normal_textures_mmap_descriptor = nullptr;
        }
    }
    
    void ces_heightmap_container_component::mmap_geometry(const std::string& filename)
    {
        ces_heightmap_container_component::erase_geometry();
        m_compressed_vertices_mmap_descriptor = std::make_shared<memory_map>();
        m_compressed_vertices = static_cast<compressed_vertex*>(m_compressed_vertices_mmap_descriptor->allocate(ces_heightmap_container_component::get_compressed_vertices_mmap_filename(filename)));
        if (!m_compressed_vertices)
        {
            assert(false);
        }
        
        m_uncompressed_vertices_mmap_descriptor = std::make_shared<memory_map>();
        m_uncompressed_vertices = static_cast<uncomressed_vertex*>(m_uncompressed_vertices_mmap_descriptor->allocate(ces_heightmap_container_component::get_uncompressed_vertices_mmap_filename(filename)));
        if (!m_uncompressed_vertices)
        {
            assert(false);
        }
        
        m_faces_mmap_descriptor = std::make_shared<memory_map>();
        m_faces = static_cast<face*>(m_faces_mmap_descriptor->allocate(ces_heightmap_container_component::get_faces_mmap_filename(filename)));
        if (!m_faces)
        {
            assert(false);
        }
        
        m_vbos_mmap_descriptor = std::make_shared<memory_map>();
        m_vbos_mmap_descriptor->allocate(ces_heightmap_container_component::get_vbos_mmap_filename(filename));
        
        ui32 offset = 0;
        for(ui32 i = 0; i < m_chunks_count.x; ++i)
        {
            for(ui32 j = 0; j < m_chunks_count.y; ++j)
            {
                m_vbos_mmap[i + j * m_chunks_count.x] = std::make_shared<mmap_heightmap_vbo>(m_vbos_mmap_descriptor);
                m_vbos_mmap[i + j * m_chunks_count.x]->set_size(m_chunk_size.x * m_chunk_size.y);
                m_vbos_mmap[i + j * m_chunks_count.x]->set_offset(offset);
                offset += m_chunk_size.x * m_chunk_size.y;
            }
        }
        
        m_ibos_mmap_descriptor = std::make_shared<memory_map>();
        m_ibos_mmap_descriptor->allocate(ces_heightmap_container_component::get_ibos_mmap_filename(filename));
        
        offset = 0;
        for(ui32 i = 0; i < m_chunks_count.x; ++i)
        {
            for(ui32 j = 0; j < m_chunks_count.y; ++j)
            {
                for(ui32 k = 0; k < e_heightmap_lod_max; ++k)
                {
                    glm::ivec2 current_chunk_size = glm::ivec2(m_chunk_lods_sizes[k].x % 2 == 0 ? m_chunk_lods_sizes[k].x : m_chunk_lods_sizes[k].x - 1,
                                                               m_chunk_lods_sizes[k].y % 2 == 0 ? m_chunk_lods_sizes[k].y : m_chunk_lods_sizes[k].y - 1);
                    
                    glm::ivec2 extended_chunk_size = current_chunk_size;
                    extended_chunk_size -= glm::ivec2(k != e_heightmap_lod_01 ? 2 : 0,
                                                      k != e_heightmap_lod_01 ? 2 : 0);
                    
                    ui32 additional_indices_count = k != e_heightmap_lod_01 ? 12 * (m_chunk_size.x - 1 + current_chunk_size.x) : 0;
                    ui32 indices_count = extended_chunk_size.x * extended_chunk_size.y * 6 + additional_indices_count;
                    
                    m_ibos_mmap[i + j * m_chunks_count.x][k] = std::make_shared<mmap_heightmap_ibo>(m_ibos_mmap_descriptor);
                    m_ibos_mmap[i + j * m_chunks_count.x][k]->set_size(indices_count);
                    m_ibos_mmap[i + j * m_chunks_count.x][k]->set_offset(offset);
                    offset += indices_count * 2;
                }
            }
        }
    }
    
    void ces_heightmap_container_component::mmap_mask_textures(const std::string& filename)
    {
        ces_heightmap_container_component::erase_mask_textures();
        
        m_splatting_mask_textures_mmap_descriptor = std::make_shared<memory_map>();
        m_splatting_mask_textures_mmap_descriptor->allocate(ces_heightmap_container_component::get_splatting_textures_mask_mmap_filename(filename));
        
        ui32 offset = 0;
        for(ui32 i = 0; i < m_chunks_count.x; ++i)
        {
            for(ui32 j = 0; j < m_chunks_count.y; ++j)
            {
                m_splatting_mask_textures_mmap[i + j * m_chunks_count.x] = std::make_shared<mmap_heightmap_RGB565>(m_splatting_mask_textures_mmap_descriptor);
                m_splatting_mask_textures_mmap[i + j * m_chunks_count.x]->set_size(ces_heightmap_container_component::k_splatting_texture_mask_size.x * ces_heightmap_container_component::k_splatting_texture_mask_size.y);
                m_splatting_mask_textures_mmap[i + j * m_chunks_count.x]->set_offset(offset);
                offset += ces_heightmap_container_component::k_splatting_texture_mask_size.x * ces_heightmap_container_component::k_splatting_texture_mask_size.y;
            }
        }
    }
    
    void ces_heightmap_container_component::mmap_diffuse_textures(const std::string& filename)
    {
        ces_heightmap_container_component::erase_diffuse_textures();
        
        m_splatting_diffuse_textures_mmap_descriptor = std::make_shared<memory_map>();
        m_splatting_diffuse_textures_mmap_descriptor->allocate(ces_heightmap_container_component::get_splatting_textures_diffuse_mmap_filename(filename));
        
        ui32 offset = 0;
        for(ui32 i = 0; i < m_chunks_count.x; ++i)
        {
            for(ui32 j = 0; j < m_chunks_count.y; ++j)
            {
                for(ui32 k = 0; k < e_heightmap_lod_max; ++k)
                {
                    m_splatting_diffuse_textures_mmap[i + j * m_chunks_count.x][k] = std::make_shared<mmap_heightmap_RGB565>(m_splatting_diffuse_textures_mmap_descriptor);
                    m_splatting_diffuse_textures_mmap[i + j * m_chunks_count.x][k]->set_size(ces_heightmap_container_component::get_textures_lod_size(static_cast<e_heightmap_lod>(k)).x *
                                                                                             ces_heightmap_container_component::get_textures_lod_size(static_cast<e_heightmap_lod>(k)).y);
                    m_splatting_diffuse_textures_mmap[i + j * m_chunks_count.x][k]->set_offset(offset);
                    offset += ces_heightmap_container_component::get_textures_lod_size(static_cast<e_heightmap_lod>(k)).x *
                    ces_heightmap_container_component::get_textures_lod_size(static_cast<e_heightmap_lod>(k)).y;
                }
            }
        }
    }
    
    void ces_heightmap_container_component::mmap_normal_textures(const std::string& filename)
    {
        ces_heightmap_container_component::erase_normal_textures();
        
        m_splatting_normal_textures_mmap_descriptor = std::make_shared<memory_map>();
        m_splatting_normal_textures_mmap_descriptor->allocate(ces_heightmap_container_component::get_splatting_textures_normal_mmap_filename(filename));
        
        ui32 offset = 0;
        for(ui32 i = 0; i < m_chunks_count.x; ++i)
        {
            for(ui32 j = 0; j < m_chunks_count.y; ++j)
            {
                for(ui32 k = 0; k < e_heightmap_lod_max; ++k)
                {
                    m_splatting_normal_textures_mmap[i + j * m_chunks_count.x][k] = std::make_shared<mmap_heightmap_RGBA8>(m_splatting_normal_textures_mmap_descriptor);
                    m_splatting_normal_textures_mmap[i + j * m_chunks_count.x][k]->set_size(ces_heightmap_container_component::get_textures_lod_size(static_cast<e_heightmap_lod>(k)).x *
                                                                                            ces_heightmap_container_component::get_textures_lod_size(static_cast<e_heightmap_lod>(k)).y *
                                                                                            ces_heightmap_container_component::k_splatting_texture_channels);
                    m_splatting_normal_textures_mmap[i + j * m_chunks_count.x][k]->set_offset(offset);
                    offset += ces_heightmap_container_component::get_textures_lod_size(static_cast<e_heightmap_lod>(k)).x *
                    ces_heightmap_container_component::get_textures_lod_size(static_cast<e_heightmap_lod>(k)).y * ces_heightmap_container_component::k_splatting_texture_channels;
                }
            }
        }
    }
}
