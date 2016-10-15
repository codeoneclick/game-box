//
//  batch.hpp
//  gbCore
//
//  Created by serhii serhiiv on 10/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef batch_hpp
#define batch_hpp

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    class batch
    {
    public:
        
        static const ui32 k_max_num_vertices;
        static const ui32 k_max_num_indices;
        
    private:
        
    protected:
        
        std::string m_guid;
        material_shared_ptr m_material;
        mesh_shared_ptr m_main_mesh;
        
        std::vector<mesh_shared_ptr> m_meshes;
        std::vector<glm::mat4x4> m_matrices;
        
        ui32 m_num_vertices_in_batch;
        ui32 m_num_indices_in_batch;
        
    public:
        
        batch(const material_shared_ptr& material);
        ~batch();
        
        void add(const mesh_shared_ptr& mesh, const glm::mat4& matrix);
        void reset();
        
        std::string get_guid() const;
        ui32 get_num_vertices() const;
        ui32 get_num_indices() const;
        
        void draw();
    };
};

#endif
