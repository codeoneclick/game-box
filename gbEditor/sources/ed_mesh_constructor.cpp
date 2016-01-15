//
//  ed_mesh_constructor.cpp
//  gbEditor
//
//  Created by sergey.sergeev on 1/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ed_mesh_constructor.h"
#include "mesh.h"
#include "vbo.h"
#include "ibo.h"

namespace gb
{
    namespace ed
    {
        mesh_shared_ptr mesh_constructor::create_grid(i32 num_rows, i32 num_columns, f32 rows_gap, f32 columns_gap)
        {
            i32 num_vertices = (num_rows + 1) * (num_columns + 1) * 4;
            i32 num_indices = (num_rows + 1) * (num_columns + 1) * 4;
            
            vbo_shared_ptr vbo = std::make_shared<gb::vbo>(num_vertices, GL_STATIC_DRAW);
            vbo::vertex_attribute* vertices = vbo->lock();
            
            i32 index = 0;
            for(i32 i = 0; i <= num_rows; ++i)
            {
                vertices[index++].m_position = glm::vec2(i * rows_gap, 0.f);
                vertices[index++].m_position = glm::vec2(i * rows_gap, num_columns * columns_gap);
            }
            
            for(i32 i = 0; i <= num_columns; ++i)
            {
                vertices[index++].m_position = glm::vec2(0.f, i * columns_gap);
                vertices[index++].m_position = glm::vec2(num_rows * rows_gap, i * columns_gap);
            }
            
            vbo->unlock();
            
            ibo_shared_ptr ibo = std::make_shared<gb::ibo>(num_indices * 4, GL_STATIC_DRAW);
            ui16* indices = ibo->lock();
            for(i32 i = 0; i < num_indices; ++i)
            {
                indices[i] = i;
            }
            ibo->unlock();
            mesh_shared_ptr mesh = std::make_shared<gb::mesh>(vbo, ibo, GL_LINES);
            return mesh;
        }
    }
}