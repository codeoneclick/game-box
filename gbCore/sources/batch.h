//
//  batch.hpp
//  gbCore
//
//  Created by serhii serhiiv on 10/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "main_headers.h"
#include "declarations.h"
#include "mesh_2d.h"
#include "vbo.h"

namespace gb
{
    class batch_cache
    {
    private:
        
        ui32 m_vbo_version;
        ui32 m_ibo_version;
        ui32 m_matrix_version;
        
        ui32 m_vbo_offset;
        ui32 m_ibo_offset;
        
    protected:
        
    public:
        
        batch_cache(ui32 vbo_version, ui32 ibo_version, ui32 matrix_version, ui32 vbo_offset, ui32 ibo_offset);
        ~batch_cache();
        
        ui32 get_vbo_version() const;
        ui32 get_ibo_version() const;
        ui32 get_matrix_version() const;
        
        ui32 get_vbo_offset() const;
        ui32 get_ibo_offset() const;
        
        void update(ui32 vbo_version, ui32 ibo_version, ui32 matrix_version, ui32 vbo_offset, ui32 ibo_offset);
    };
    
    class batch
    {
    public:
        
        static const ui32 k_max_num_vertices;
        static const ui32 k_max_num_indices;
        
    private:
        
        bool m_is_vbo_changed;
        bool m_is_ibo_changed;
        
    protected:
        
        std::string m_guid;
        material_shared_ptr m_material;
        mesh_2d_shared_ptr m_batch;
        
        ui32 m_num_vertices_in_batch;
        ui32 m_num_indices_in_batch;
        
        std::unordered_map<ui32, std::shared_ptr<batch_cache>> m_cache;
        
    public:
        
        batch(const material_shared_ptr& material);
        ~batch();
        
        void add(const mesh_2d_shared_ptr& mesh, const glm::mat4& matrix, ui32 matrix_version);
        void reset();
        
        std::string get_guid() const;
        ui32 get_num_vertices() const;
        ui32 get_num_indices() const;
        
        void draw();
    };
};

#endif
