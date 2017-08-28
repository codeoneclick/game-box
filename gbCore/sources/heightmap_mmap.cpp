//
//  heightmap_mmap.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "heightmap_mmap.h"
#include "common.h"

#if defined(__IOS__)

#import <UIKit/UIKit.h>

#elif defined(__OSX__)

#include <Cocoa/Cocoa.h>

#endif


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
    
    heightmap_mmap::heightmap_mmap(const std::string& filename) :
    m_filename(filename)
    {
        heightmap_mmap::load_from_source_filename();
        
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
    
    void heightmap_mmap::load_from_source_filename()
    {
        ui8* data = nullptr;
        
#if defined(__IOS__)
        
        UIImage* image = [UIImage imageNamed:[NSString stringWithCString:m_filename.c_str() encoding:NSUTF8StringEncoding]];
        CGColorSpaceRef colorSpace = CGColorSpaceCreateDeviceRGB();
        size_t bytesPerRow = image.size.width * 4;
        data = (ui8 *)malloc(image.size.height * bytesPerRow);
        CGContextRef context = CGBitmapContextCreate(data,
                                                     image.size.width,
                                                     image.size.height,
                                                     8,
                                                     bytesPerRow,
                                                     colorSpace,
                                                     kCGImageAlphaNoneSkipFirst);
        UIGraphicsPushContext(context);
        CGContextTranslateCTM(context, 0.0, image.size.height);
        CGContextScaleCTM(context, 1.0, -1.0);
        [image drawInRect:CGRectMake(0.0, 0.0, image.size.width, image.size.height)];
        UIGraphicsPopContext();
        m_heightmap_size = glm::ivec2(image.size.width, image.size.height);
        
#elif defined(__OSX__)
        
        NSImage* image = [NSImage imageNamed:[NSString stringWithCString:m_filename.c_str() encoding:NSUTF8StringEncoding]];
        CGImageSourceRef source = CGImageSourceCreateWithData((__bridge CFDataRef)[image TIFFRepresentation], NULL);
        CGImageRef mask =  CGImageSourceCreateImageAtIndex(source, 0, NULL);
        NSBitmapImageRep *bitmap = [[NSBitmapImageRep alloc] initWithCGImage:mask];
        data = [bitmap bitmapData];
        m_heightmap_size = glm::ivec2(image.size.width, image.size.height);
        CFRelease(source);
        CFRelease(mask);
        
        
#endif
        
        m_heights.resize(m_heightmap_size.x * m_heightmap_size.y);
        ui32 index = 0;
        for(ui32 i = 0; i < m_heightmap_size.x; ++i)
        {
            for(ui32 j = 0; j < m_heightmap_size.y; ++j)
            {
                m_heights[index++] = static_cast<f32>(data[(i + j * m_heightmap_size.x) * 4 + 1]) / 255.f * heightmap_constants::k_raise - heightmap_constants::k_deep;
            }
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
    
    std::string heightmap_mmap::get_filename() const
    {
        return m_filename;
    }
    
    const std::vector<f32>& heightmap_mmap::get_heights() const
    {
        return m_heights;
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
    
    void heightmap_mmap::attach_uncompressed_vertex_to_vbo(i32 i, i32 j, ui32 vbo_index, ui32 vbo_vertex_index)
    {
        i32 index = i + j * m_heightmap_size.x;
        assert(m_uncompressed_vertices[index].m_contains_in_vbo_size <= heightmap_constants::k_max_vertices_contains_in_vbo);
        m_uncompressed_vertices[index].m_contains_in_vbo[m_uncompressed_vertices[index].m_contains_in_vbo_size++] = glm::ivec2(vbo_index, vbo_vertex_index);
    }
    
    const std::array<glm::ivec2, heightmap_constants::k_max_vertices_contains_in_vbo>& heightmap_mmap::attached_vertices_to_vbo(i32 i, i32 j, ui8 *size) const
    {
        i32 index = i + j * m_heightmap_size.x;
        if(m_uncompressed_vertices[index].m_contains_in_vbo_size == 0 || m_uncompressed_vertices[index].m_contains_in_vbo_size > heightmap_constants::k_max_vertices_contains_in_vbo)
        {
            assert(m_uncompressed_vertices[index].m_contains_in_vbo_size != 0 && m_uncompressed_vertices[index].m_contains_in_vbo_size <= heightmap_constants::k_max_vertices_contains_in_vbo);
            *size = 0;
            static std::array<glm::ivec2, heightmap_constants::k_max_vertices_contains_in_vbo> zero_value;
            return zero_value;
        }
        *size = m_uncompressed_vertices[index].m_contains_in_vbo_size;
        return m_uncompressed_vertices[index].m_contains_in_vbo;
    }
    
    void heightmap_mmap::attach_uncompressed_vertex_to_face(i32 i, i32 j, ui32 face_index)
    {
        i32 index = i + j * m_heightmap_size.x;
        assert(m_uncompressed_vertices[index].m_contains_in_face_size <= heightmap_constants::k_max_vertices_contains_in_face);
        m_uncompressed_vertices[index].m_contains_in_face[m_uncompressed_vertices[index].m_contains_in_face_size++] = face_index;
    }
    
    std::array<ui32, heightmap_constants::k_max_vertices_contains_in_face> heightmap_mmap::attached_vertices_to_face(i32 i, i32 j, ui8 *size) const
    {
        i32 index = i + j * m_heightmap_size.x;
        assert(m_uncompressed_vertices[index].m_contains_in_face_size != 0 && m_uncompressed_vertices[index].m_contains_in_face_size <= heightmap_constants::k_max_vertices_contains_in_face);
        *size = m_uncompressed_vertices[index].m_contains_in_face_size;
        return m_uncompressed_vertices[index].m_contains_in_face;
    }
    
    glm::vec3 heightmap_mmap::get_vertex_position(ui32 i, ui32 j) const
    {
        return m_compressed_vertices[i + j * m_heightmap_size.x].m_position;
    };
    
    glm::uint32 heightmap_mmap::get_compressed_vertex_texcoord(ui32 i, ui32 j) const
    {
        return m_compressed_vertices[i + j * m_heightmap_size.x].m_texcoord;
    };
    
    glm::vec2 heightmap_mmap::get_uncompressed_vertex_texcoord(ui32 i, ui32 j) const
    {
        return glm::unpackUnorm2x16(m_compressed_vertices[i + j * m_heightmap_size.x].m_texcoord);
    };
    
    glm::uint32 heightmap_mmap::get_compressed_vertex_normal(ui32 i, ui32 j) const
    {
        return m_compressed_vertices[i + j * m_heightmap_size.x].m_normal;
    };
    
    glm::vec3 heightmap_mmap::get_uncompressed_vertex_normal(ui32 i, ui32 j) const
    {
        glm::vec4 normal = glm::unpackSnorm4x8(m_compressed_vertices[i + j * m_heightmap_size.x].m_normal);
        return glm::vec3(normal.x, normal.y, normal.z);
    };
}
