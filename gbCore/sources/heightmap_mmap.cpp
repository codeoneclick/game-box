//
//  heightmap_mmap.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "heightmap_mmap.h"
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
    
    heightmap_mmap::heightmap_mmap(const glm::ivec2& heightmap_size) :
    m_heightmap_size(heightmap_size)
    {
        m_uncompressed_vertices = new uncomressed_vertex[m_heightmap_size.x * m_heightmap_size.y];
        m_compressed_vertices = new compressed_vertex[m_heightmap_size.x * m_heightmap_size.y];
        m_faces = new face[(m_heightmap_size.x - 1) * (m_heightmap_size.y - 1) * 2];
        
        m_compressed_vertices_mmap_descriptor = std::make_shared<memory_map>();
        m_uncompressed_vertices_mmap_descriptor = std::make_shared<memory_map>();
        m_faces_mmap_descriptor = std::make_shared<memory_map>();
        m_vbos_mmap_descriptor = std::make_shared<memory_map>();
        m_ibos_mmap_descriptor = std::make_shared<memory_map>();
        m_splatting_mask_textures_mmap_descriptor = std::make_shared<memory_map>();
        m_splatting_diffuse_textures_mmap_descriptor = std::make_shared<memory_map>();
        m_splatting_normal_textures_mmap_descriptor = std::make_shared<memory_map>();
    }
    
    heightmap_mmap::~heightmap_mmap()
    {
        if(m_uncompressed_vertices_mmap_descriptor != nullptr)
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

        if(m_splatting_mask_textures_mmap_descriptor)
        {
            m_splatting_mask_textures_mmap_descriptor->deallocate();
            m_splatting_mask_textures_mmap_descriptor = nullptr;
        }
        
        if(m_splatting_diffuse_textures_mmap_descriptor)
        {
            m_splatting_diffuse_textures_mmap_descriptor->deallocate();
            m_splatting_diffuse_textures_mmap_descriptor = nullptr;
        }
        
        if(m_splatting_normal_textures_mmap_descriptor)
        {
            m_splatting_normal_textures_mmap_descriptor->deallocate();
            m_splatting_normal_textures_mmap_descriptor = nullptr;
        }
    }
    
    std::string heightmap_mmap::get_uncompressed_vertices_mmap_filename(const std::string &filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_uncompressed_vertices_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string heightmap_mmap::get_compressed_vertices_mmap_filename(const std::string& filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_compressed_vertices_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string heightmap_mmap::get_faces_mmap_filename(const std::string& filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_faces_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string heightmap_mmap::get_vbos_mmap_filename(const std::string &filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_vbos_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string heightmap_mmap::get_ibos_mmap_filename(const std::string &filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_ibos_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string heightmap_mmap::get_splatting_textures_mask_mmap_filename(const std::string &filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_splatting_textures_mask_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string heightmap_mmap::get_splatting_textures_normal_mmap_filename(const std::string& filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_splatting_textures_normal_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string heightmap_mmap::get_splatting_textures_diffuse_mmap_filename(const std::string& filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_splatting_textures_diffuse_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string heightmap_mmap::get_tangent_space_mmap_filename(const std::string& filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_tangent_space_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    std::string heightmap_mmap::get_attaches_to_vbo_mmap_filename(const std::string &filename)
    {
        std::ostringstream stringstream;
        stringstream<<filename<<k_attaches_to_vbo_data_filename;
        
#if defined(__IOS__)
        
        return documentspath() + stringstream.str();
        
#endif
        
        return stringstream.str();
    }
    
    bool heightmap_mmap::is_uncompressed_vertices_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(heightmap_mmap::get_uncompressed_vertices_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool heightmap_mmap::is_compressed_vertices_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(heightmap_mmap::get_compressed_vertices_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool heightmap_mmap::is_faces_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(heightmap_mmap::get_faces_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool heightmap_mmap::is_vbos_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(heightmap_mmap::get_vbos_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool heightmap_mmap::is_ibos_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(heightmap_mmap::get_ibos_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool heightmap_mmap::is_splatting_textures_mask_mmap_exist(const std::string &filename)
    {
        std::ifstream stream(heightmap_mmap::get_splatting_textures_mask_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool heightmap_mmap::is_splatting_textures_normal_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(heightmap_mmap::get_splatting_textures_normal_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool heightmap_mmap::is_splatting_textures_diffuse_mmap_exist(const std::string& filename)
    {
        std::ifstream stream(heightmap_mmap::get_splatting_textures_diffuse_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool heightmap_mmap::is_tangent_space_mmap_exist(const std::string &filename)
    {
        std::ifstream stream(heightmap_mmap::get_tangent_space_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    bool heightmap_mmap::is_attaches_to_vbo_mmap_exist(const std::string &filename)
    {
        std::ifstream stream(heightmap_mmap::get_attaches_to_vbo_mmap_filename(filename));
        bool isExist = stream.good();
        stream.close();
        return isExist;
    }
    
    glm::ivec2 heightmap_mmap::get_heightmap_size() const
    {
        return m_heightmap_size;
    }
    
    heightmap_mmap::uncomressed_vertex* heightmap_mmap::get_uncopressed_vertices() const
    {
        return m_uncompressed_vertices;
    }
    
    heightmap_mmap::compressed_vertex* heightmap_mmap::get_compressed_vertices() const
    {
        return m_compressed_vertices;
    }
    
    heightmap_mmap::face* heightmap_mmap::get_faces() const
    {
        return m_faces;
    }
    
    void heightmap_mmap::set_uncompressed_vertices(uncomressed_vertex* uncompressed_vertices)
    {
        delete[] m_uncompressed_vertices;
        m_uncompressed_vertices = uncompressed_vertices;
    }
    
    void heightmap_mmap::set_compressed_vertices(compressed_vertex* compressed_vertices)
    {
        delete[] m_compressed_vertices;
        m_compressed_vertices = compressed_vertices;
    }
    
    void heightmap_mmap::set_faces(face* faces)
    {
        delete[] m_faces;
        m_faces = faces;
    }
    
    std::shared_ptr<memory_map> heightmap_mmap::get_uncompressed_vertices_mmap_descriptor()
    {
        return m_uncompressed_vertices_mmap_descriptor;
    }
    
    std::shared_ptr<memory_map> heightmap_mmap::get_faces_mmap_descriptor()
    {
        return m_faces_mmap_descriptor;
    }
    
    std::shared_ptr<memory_map> heightmap_mmap::get_compressed_vertices_mmap_descriptor()
    {
        return m_compressed_vertices_mmap_descriptor;
    }
    
    std::shared_ptr<memory_map> heightmap_mmap::get_vbos_mmap_descriptor()
    {
        return m_vbos_mmap_descriptor;
    }
    
    std::shared_ptr<memory_map> heightmap_mmap::get_ibos_mmap_descriptor()
    {
        return m_ibos_mmap_descriptor;
    }
    
    std::shared_ptr<memory_map> heightmap_mmap::get_splatting_mask_textures_mmap_descriptor()
    {
        return m_splatting_mask_textures_mmap_descriptor;
    }
    
    std::shared_ptr<memory_map> heightmap_mmap::get_splatting_diffuse_textures_mmap_descriptor()
    {
        return m_splatting_diffuse_textures_mmap_descriptor;
    }
    
    std::shared_ptr<memory_map> heightmap_mmap::get_splatting_normal_textures_mmap_descriptor()
    {
        return m_splatting_normal_textures_mmap_descriptor;
    }
}
