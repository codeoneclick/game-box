//
//  mesh_constructor.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "mesh_constructor.h"
#include "mesh.h"
#include "vbo.h"
#include "ibo.h"
#include "std_extensions.h"

namespace gb
{
    std::once_flag g_screen_quad_created;
    std::once_flag g_shape_quad_created;
    
    mesh_shared_ptr mesh_constructor::create_screen_quad()
    {
        static mesh_shared_ptr mesh = nullptr;
        std::call_once(g_screen_quad_created, [] {
            
            vbo_shared_ptr vbo = std::make_shared<gb::vbo>(4, GL_STATIC_DRAW);
            vbo::vertex_attribute *vertices = vbo->lock();
            
            vertices[0].m_position = glm::vec2(-1.f, -1.f);
            vertices[0].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f, 0.f));
            vertices[1].m_position = glm::vec2(-1.f, 1.f);
            vertices[1].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f, 1.f));
            vertices[2].m_position = glm::vec2(1.f, -1.f);
            vertices[2].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f, 0.f));
            vertices[3].m_position = glm::vec2(1.f, 1.f);
            vertices[3].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f, 1.f));
            vbo->unlock();
            
            ibo_shared_ptr ibo = std::make_shared<gb::ibo>(6, GL_STATIC_DRAW);
            ui16* indices = ibo->lock();
            indices[0] = 0;
            indices[1] = 1;
            indices[2] = 2;
            indices[3] = 1;
            indices[4] = 2;
            indices[5] = 3;
            ibo->unlock();
            
            mesh = std::make_shared<gb::mesh>(vbo, ibo);
        });
        return mesh;
    }
    
    mesh_shared_ptr mesh_constructor::create_shape_quad()
    {
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(4, GL_STATIC_DRAW);
        vbo::vertex_attribute *vertices = vbo->lock();
        
        vertices[0].m_position = glm::vec2(-.5f, -.5f);
        vertices[0].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f, 0.f));
        vertices[1].m_position = glm::vec2(-.5f, .5f);
        vertices[1].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f, 1.f));
        vertices[2].m_position = glm::vec2(.5f, -.5f);
        vertices[2].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f, 0.f));
        vertices[3].m_position = glm::vec2(.5f, .5f);
        vertices[3].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f, 1.f));
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(6, GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        indices[0] = 0;
        indices[1] = 2;
        indices[2] = 1;
        indices[3] = 1;
        indices[4] = 2;
        indices[5] = 3;
        ibo->unlock();
        
        mesh_shared_ptr mesh = std::make_shared<gb::mesh>(vbo, ibo);
        return mesh;
    }
    
    mesh_shared_ptr mesh_constructor::create_circle()
    {
        i32 num_subdivisions = 32;
        f32 radius = 64.f;
        
        i32 num_vertices = num_subdivisions + 1;
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(num_vertices, GL_STATIC_DRAW);
        vbo::vertex_attribute *vertices = vbo->lock();
        
        vertices[0].m_position = glm::vec2(0.f, 0.f);
        
        i32 index = 1;
        for(f32 angle=0; angle <= M_PI * 2.f; angle += ((M_PI * 2) / num_subdivisions))
        {
            vertices[index++].m_position = glm::vec2(radius * cosf(angle),
                                                     radius * sinf(angle));
        }
        vbo->unlock();
        
        index = 1;
        i32 num_indices = (num_subdivisions + 1) * 3;
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(num_indices, GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        for (i32 i = 0; i < num_subdivisions * 3; i += 3)
        {
            indices[i + 0] = 0;
            indices[i + 1] = index++;
            indices[i + 2] = index;
        }
        
        indices[num_indices - 3] = 0;
        indices[num_indices - 2] = index - 1;
        indices[num_indices - 1] = 1;
        ibo->unlock();
        
        mesh_shared_ptr mesh = std::make_shared<gb::mesh>(vbo, ibo);
        return mesh;
    }
}