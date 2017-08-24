//
//  heightmap_container.h
//  gbCore
//
//  Created by serhii serhiiv on 8/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "heightmap_constants.h"
#include "memory_map.h"
#include "vbo.h"

namespace gb
{
    class heightmap_container
    {
    public:
        
        struct uncomressed_vertex
        {
            glm::vec3 m_position;
            glm::vec3 m_normal;
            glm::vec2 m_texcoord;
            
            std::array<ui32, heightmap_constants::k_max_vertices_contains_in_face> m_contains_in_face;
            ui8 m_contains_in_face_size;
            
            std::array<glm::ivec2, heightmap_constants::k_max_vertices_contains_in_vbo> m_contains_in_vbo;
            ui8 m_contains_in_vbo_size;
            
            uncomressed_vertex()
            {
                m_contains_in_face_size = 0;
                m_contains_in_vbo_size = 0;
                
                m_contains_in_face.fill(0);
                m_contains_in_vbo.fill(glm::ivec2(0));
            };
            
            ~uncomressed_vertex() = default;
            
            uncomressed_vertex(const uncomressed_vertex& copy) = delete;
            uncomressed_vertex(uncomressed_vertex&& copy) = delete;
            uncomressed_vertex& operator = (const uncomressed_vertex& copy) = delete;
            uncomressed_vertex& operator = (uncomressed_vertex&& copy) = delete;
        };
        
        struct compressed_vertex
        {
            glm::vec3 m_position;
            glm::uint32 m_normal;
            glm::uint32 m_texcoord;
            
            compressed_vertex() = default;
            ~compressed_vertex() = default;
        };
        
        struct face
        {
            glm::vec3 m_normal;
            std::array<ui32, 3> m_indexes;
            
            face()
            {
                m_indexes.fill(0);
            }
            ~face() = default;
            
            face(const face& copy) = delete;
            face(face&& copy) = delete;
            face& operator = (const face& copy) = delete;
            face& operator = (face&& copy) = delete;
        };
        
        class mmap_heightmap_base
        {
        private:
            
        protected:
            
            std::shared_ptr<memory_map> m_descriptor;
            
            ui32 m_size;
            ui32 m_offset;
            
        public:
            
            mmap_heightmap_base(const std::shared_ptr<memory_map>& descriptor) :
            m_descriptor(descriptor),
            m_size(0),
            m_offset(0)
            {
                
            }
            virtual ~mmap_heightmap_base() = default;
            
            inline void set_size(ui32 size) { m_size = size; };
            inline void set_offset(ui32 offset) { m_offset = offset; }
            inline ui32 get_size(void) const { return m_size; };
        };
        
        class mmap_heightmap_vbo : public mmap_heightmap_base
        {
        private:
            
        protected:
            
        public:
            
            mmap_heightmap_vbo(const std::shared_ptr<memory_map>& descriptor) : mmap_heightmap_base(descriptor)
            {
                
            };
            ~mmap_heightmap_vbo() = default;
            
            inline vbo::vertex_attribute_PTNTCE* get_pointer() const
            {
                vbo::vertex_attribute_PTNTCE* pointer = (vbo::vertex_attribute_PTNTCE* )m_descriptor->pointer();
                assert(pointer != nullptr);
                return pointer + m_offset;
            };
        };
        
        class mmap_heightmap_ibo : public mmap_heightmap_base
        {
        private:
            
            
        protected:
            
        public:
            
            mmap_heightmap_ibo(const std::shared_ptr<memory_map>& descriptor) : mmap_heightmap_base(descriptor)
            {
                
            };
            ~mmap_heightmap_ibo() = default;
            
            inline ui16* get_source_pointer() const
            {
                ui16* pointer = (ui16* )m_descriptor->pointer();
                assert(pointer != nullptr);
                return pointer + m_offset;
            };
            
            inline ui16* get_origin_pointer() const
            {
                ui16* pointer = (ui16* )m_descriptor->pointer();
                assert(pointer != nullptr);
                return pointer + m_offset + m_size;
            };
            
            inline void update_source_pointer()
            {
                ui16* source_pointer = mmap_heightmap_ibo::get_source_pointer();
                ui16* origin_pointer = mmap_heightmap_ibo::get_origin_pointer();
                memcpy(source_pointer, origin_pointer, m_size * sizeof(ui16));
            }
        };
        
        class mmap_heightmap_RGB565 : public mmap_heightmap_base
        {
        private:
            
        protected:
            
        public:
            
            mmap_heightmap_RGB565(const std::shared_ptr<memory_map>& descriptor) : mmap_heightmap_base(descriptor) { };
            ~mmap_heightmap_RGB565() = default;
            
            inline ui16* get_pointer() const
            {
                ui16* pointer = (ui16* )m_descriptor->pointer();
                assert(pointer != nullptr);
                return pointer + m_offset;
            };
        };
        
        class mmap_heightmap_RGBA8 : public mmap_heightmap_base
        {
        private:
            
        protected:
            
        public:
            
            mmap_heightmap_RGBA8(const std::shared_ptr<memory_map>& descriptor) : mmap_heightmap_base(descriptor) { };
            ~mmap_heightmap_RGBA8() = default;
            
            inline ui8* get_pointer() const
            {
                ui8* pointer = (ui8* )m_descriptor->pointer();
                assert(pointer != nullptr);
                return pointer + m_offset;
            };
        };
        
    private:
        
        glm::ivec2 m_heightmap_size;
        
        uncomressed_vertex* m_uncompressed_vertices;
        face* m_faces;
        compressed_vertex* m_compressed_vertices;
        
        std::shared_ptr<memory_map> m_uncompressed_vertices_mmap_descriptor;
        std::shared_ptr<memory_map> m_faces_mmap_descriptor;
        std::shared_ptr<memory_map> m_compressed_vertices_mmap_descriptor;
        
        std::shared_ptr<memory_map> m_vbos_mmap_descriptor;
        std::shared_ptr<memory_map> m_ibos_mmap_descriptor;
        
        std::shared_ptr<memory_map> m_splatting_mask_textures_mmap_descriptor;
        std::shared_ptr<memory_map> m_splatting_diffuse_textures_mmap_descriptor;
        std::shared_ptr<memory_map> m_splatting_normal_textures_mmap_descriptor;
        
    protected:
        
    public:
        
        heightmap_container(const glm::ivec2& heightmap_size);
        ~heightmap_container();
        
        uncomressed_vertex* get_uncopressed_vertices() const;
        compressed_vertex* get_compressed_vertices() const;
        face* get_faces() const;
    };
};

