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
#include "instanced_mesh.h"
#include "std_extensions.h"

namespace gb
{
    mesh_shared_ptr mesh_constructor::create_wireframe_box(const glm::vec3& min_bound,
                                                           const glm::vec3& max_bound)
    {
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(8, GL_STATIC_DRAW);
        vbo::vertex_attribute* vertices = vbo->lock();
        
        glm::vec3 middle = (min_bound + max_bound) * .5f;
        glm::vec3 min_coords = middle + (min_bound - middle) * 1.01f;
        glm::vec3 max_coords = middle + (max_bound - middle) * 1.01f;
        
        vertices[0].m_position = min_coords;
        vertices[1].m_position = glm::vec3(max_coords.x, min_coords.y, min_coords.z);
        vertices[2].m_position = glm::vec3(max_coords.x, max_coords.y, min_coords.z);
        vertices[3].m_position = glm::vec3(min_coords.x, max_coords.y, min_coords.z);
        
        vertices[4].m_position = glm::vec3(min_coords.x, min_coords.y, max_coords.z);
        vertices[5].m_position = glm::vec3(max_coords.x, min_coords.y, max_coords.z);
        vertices[6].m_position = max_coords;
        vertices[7].m_position = glm::vec3(min_coords.x, max_coords.y, max_coords.z);
        
        vbo->unlock();
        
        GLushort wireframe_indices[] =
        {
            0, 1, 1, 2,	2, 3, 3, 0,
            4, 5, 5, 6, 6, 7, 7, 4,
            0, 4, 1, 5, 2, 6, 3, 7
        };
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(24, GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        memcpy(indices, wireframe_indices, sizeof(wireframe_indices));
        ibo->unlock();
        
        mesh_shared_ptr mesh = gb::mesh::construct("wireframe_box",
                                                   vbo, ibo,
                                                   min_bound, max_bound,
                                                   GL_LINES);
        return mesh;
    }
    
    mesh_shared_ptr mesh_constructor::create_plane(const glm::vec2& size)
    {
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(4, GL_STATIC_DRAW);
        vbo::vertex_attribute* vertices = vbo->lock();
        
        vertices[0].m_position = glm::vec3(0.f);
        vertices[1].m_position = glm::vec3(size.x, 0.f, 0.f);
        vertices[2].m_position = glm::vec3(size.x, 0.f, size.y);
        vertices[3].m_position = glm::vec3(0.f, 0.f, size.y);
        
        vertices[0].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
        vertices[1].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
        vertices[2].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
        vertices[3].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
        
        vertices[0].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f,  0.f));
        vertices[1].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f,  0.f));
        vertices[2].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f,  1.f));
        vertices[3].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f,  1.f));
        
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(6, GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 0;
        indices[4] = 2;
        indices[5] = 3;
        
        ibo->unlock();
        
        mesh_shared_ptr mesh = gb::mesh::construct("plane", vbo, ibo,
                                                   glm::vec3(0.f), glm::vec3(size.x, 0.f, size.y));
        return mesh;
    }
    
    std::tuple<vbo_shared_ptr, ibo_shared_ptr, glm::vec3, glm::vec3> mesh_constructor::create_sphere_data(f32 radius, i32 rings, i32 sectors)
    {
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>((rings + 1) * (sectors + 1), GL_STATIC_DRAW);
        vbo::vertex_attribute* vertices = vbo->lock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>((rings * sectors + rings) * 6, GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        
        i32 index = 0;
        for(i32 i = 0; i <= sectors; ++i)
        {
            f32 v = i / static_cast<f32>(sectors);
            f32 phi = v * M_PI;
            
            for (i32 j = 0; j <= rings; ++j )
            {
                f32 u = j / static_cast<f32>(rings);
                f32 theta = u * M_PI * 2.f;
                
                f32 x = cos(theta) * sin(phi);
                f32 y = cos(phi);
                f32 z = sin(theta) * sin(phi);
                
                vertices[index].m_position = glm::vec3(x, y, z) * radius;
                vertices[index].m_normal = glm::packSnorm4x8(glm::vec4(x, y, z, 0.f));
                vertices[index].m_texcoord =  glm::packUnorm2x16(glm::vec2(u, v));
                
                ++index;
            }
        }
        
        index = 0;
        for(i32 i = 0; i < rings * sectors + rings; ++i)
        {
            indices[index++] = i;
            indices[index++] = i + rings + 1;
            indices[index++] = i + rings;
            
            indices[index++] = i + rings + 1;
            indices[index++] = i;
            indices[index++] = i + 1;
        }
        
        vbo->unlock();
        ibo->unlock();
        
        return std::make_tuple(std::move(vbo), std::move(ibo), std::move( glm::vec3(-radius / 2.f)), std::move(glm::vec3(radius / 2.f)));
    }
    
    std::tuple<vbo_shared_ptr, ibo_shared_ptr> mesh_constructor::create_box_data(const glm::vec3& min_bound,
                                                                                 const glm::vec3& max_bound)
    {
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(24, GL_STATIC_DRAW);
        vbo::vertex_attribute* vertices = vbo->lock();
        
        f32 raw_vertices[3 * 4 * 6] =
        {
            // front
            min_bound.x, min_bound.y, max_bound.z,
            max_bound.x, min_bound.y, max_bound.z,
            max_bound.x, max_bound.y, max_bound.z,
            min_bound.x, max_bound.y, max_bound.z,
            // top
            min_bound.x, max_bound.y, max_bound.z,
            max_bound.x, max_bound.y, max_bound.z,
            max_bound.x, max_bound.y, min_bound.z,
            min_bound.x, max_bound.y, min_bound.z,
            // back
            max_bound.x, min_bound.y, min_bound.z,
            min_bound.x, min_bound.y, min_bound.z,
            min_bound.x, max_bound.y, min_bound.z,
            max_bound.x, max_bound.y, min_bound.z,
            // bottom
            min_bound.x, min_bound.y, min_bound.z,
            max_bound.x, min_bound.y, min_bound.z,
            max_bound.x, min_bound.y, max_bound.z,
            min_bound.x, min_bound.y, max_bound.z,
            // left
            min_bound.x, min_bound.y, min_bound.z,
            min_bound.x, min_bound.y, max_bound.z,
            min_bound.x, max_bound.y, max_bound.z,
            min_bound.x, max_bound.y, min_bound.z,
            // right
            max_bound.x, min_bound.y, max_bound.z,
            max_bound.x, min_bound.y, min_bound.z,
            max_bound.x, max_bound.y, min_bound.z,
            max_bound.x, max_bound.y, max_bound.z,
        };
        
        f32 raw_normals[4 * 4 * 6] =
        {
            // front
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            0.f, 0.f, 1.f, 0.f,
            // top
            0.f, 1.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            0.f, 1.f, 0.f, 0.f,
            // back
            0.f, 0.f, -1.f, 0.f,
            0.f, 0.f, -1.f, 0.f,
            0.f, 0.f, -1.f, 0.f,
            0.f, 0.f, -1.f, 0.f,
            // bottom
            0.f, -1.f, 0.f, 0.f,
            0.f, -1.f, 0.f, 0.f,
            0.f, -1.f, 0.f, 0.f,
            0.f, -1.f, 0.f, 0.f,
            // left
            -1.f, 0.f, 0.f, 0.f,
            -1.f, 0.f, 0.f, 0.f,
            -1.f, 0.f, 0.f, 0.f,
            -1.f, 0.f, 0.f, 0.f,
            // right
            1.f, 0.f,  0.f, 0.f,
            1.f, 0.f,  0.f, 0.f,
            1.f, 0.f,  0.f, 0.f,
            1.f, 0.f,  0.f, 0.f,
        };
        
        f32 raw_texcoords[2 * 4 * 6] =
        {
            // front
            0.f, 0.f,
            1.f, 0.f,
            1.f, 1.f,
            0.f, 1.f,
        };
        
        for(i32 i = 1; i < 6; i++)
        {
            memcpy(&raw_texcoords[i * 4 * 2], &raw_texcoords[0], 2 * 4 * sizeof(f32));
        }
        
        ui16 raw_indices[36] =
        {
            // front
            0,  1,  2,
            2,  3,  0,
            // top
            4,  5,  6,
            6,  7,  4,
            // back
            8,  9, 10,
            10, 11, 8,
            // bottom
            12, 13, 14,
            14, 15, 12,
            // left
            16, 17, 18,
            18, 19, 16,
            // right
            20, 21, 22,
            22, 23, 20,
        };
        
        for (i32 i = 0; i < 24; ++i)
        {
            vertices[i].m_position = glm::vec3(raw_vertices[i * 3 + 0],
                                               raw_vertices[i * 3 + 1],
                                               raw_vertices[i * 3 + 2]);
            
            vertices[i].m_texcoord = glm::packUnorm2x16(glm::vec2(raw_texcoords[i * 2 + 0],
                                                                  raw_texcoords[i * 2 + 1]));
            
            vertices[i].m_normal = glm::packSnorm4x8(glm::vec4(raw_normals[i * 4 + 0],
                                                               raw_normals[i * 4 + 1],
                                                               raw_normals[i * 4 + 2],
                                                               raw_normals[i * 4 + 3]));
        }
        
        
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(36, GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        memcpy(indices, raw_indices, sizeof(ui16) * 36);
        
        std::vector<std::pair<glm::vec3, ui32>> tangents;
        tangents.resize(24, std::make_pair(glm::vec3(0.0f), 0));
        
        for (ui32 i = 0; i < 36; i += 3)
        {
            glm::vec3 point_01 = vertices[indices[i + 0]].m_position;
            glm::vec3 point_02 = vertices[indices[i + 1]].m_position;
            glm::vec3 point_03 = vertices[indices[i + 2]].m_position;
            
            glm::vec2 texcoord_01 = glm::unpackUnorm2x16(vertices[indices[i + 0]].m_texcoord);
            glm::vec2 texcoord_02 = glm::unpackUnorm2x16(vertices[indices[i + 1]].m_texcoord);
            glm::vec2 texcoord_03 = glm::unpackUnorm2x16(vertices[indices[i + 2]].m_texcoord);
            
            glm::vec3 tangent = mesh_constructor::generate_tangent(point_01, point_02, point_03,
                                                                   texcoord_01, texcoord_02, texcoord_03);
            
            tangents[indices[i + 0]].first += tangent;
            tangents[indices[i + 0]].second++;
            
            tangents[indices[i + 1]].first += tangent;
            tangents[indices[i + 1]].second++;
            
            tangents[indices[i + 2]].first += tangent;
            tangents[indices[i + 2]].second++;
        }
        
        for(i32 i = 0; i < tangents.size(); ++i)
        {
            glm::vec3 tangent = tangents[i].first / static_cast<f32>(tangents[i].second);
            glm::vec4 normal = glm::unpackSnorm4x8(vertices[i].m_normal);
            tangent = mesh_constructor::ortogonalize(glm::vec3(normal.x, normal.y, normal.z), tangent);
            vertices[i].m_tangent = glm::packSnorm4x8(glm::vec4(tangent.x, tangent.y, tangent.z, 0.0));
        }
        
        vbo->unlock();
        ibo->unlock();
        
        return std::make_tuple(vbo, ibo);
    }
    
    mesh_shared_ptr mesh_constructor::create_sphere(f32 radius, i32 rings, i32 sectors)
    {
        std::tuple<vbo_shared_ptr, ibo_shared_ptr, glm::vec3, glm::vec3> data = mesh_constructor::create_sphere_data(radius, rings, sectors);
        mesh_shared_ptr mesh = gb::mesh::construct("sphere",
                                                   std::get<0>(data), std::get<1>(data),
                                                   std::get<2>(data), std::get<3>(data));
        return mesh;
    }
    
    mesh_shared_ptr mesh_constructor::create_screen_quad()
    {
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(4, GL_STATIC_DRAW);
        vbo::vertex_attribute *vertices = vbo->lock();
        
        vertices[0].m_position = glm::vec3(-1.0f, -1.0f, 0.0f);
        vertices[0].m_texcoord = glm::packUnorm2x16(glm::vec2(0.0f, 0.0f));
        vertices[1].m_position = glm::vec3(-1.0f, 1.0f, 0.0f);
        vertices[1].m_texcoord = glm::packUnorm2x16(glm::vec2(0.0f, 1.0f));
        vertices[2].m_position = glm::vec3(1.0f, -1.0f, 0.0f);
        vertices[2].m_texcoord = glm::packUnorm2x16(glm::vec2(1.0f, 0.0f));
        vertices[3].m_position = glm::vec3(1.0f, 1.0f, 0.0f);
        vertices[3].m_texcoord = glm::packUnorm2x16(glm::vec2(1.0f, 1.0f));
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
        
        mesh_shared_ptr mesh = gb::mesh::construct("screen_quad", vbo, ibo,
                                                   glm::vec3(-4096.f),
                                                   glm::vec3( 4096.f));
        return mesh;
    }
    
    instanced_mesh_shared_ptr mesh_constructor::create_instanced_sphere(f32 radius, i32 rings, i32 sectors)
    {
        std::tuple<vbo_shared_ptr, ibo_shared_ptr, glm::vec3, glm::vec3> data = mesh_constructor::create_sphere_data(radius, rings, sectors);
        instanced_mesh_shared_ptr mesh = instanced_mesh::construct("sphere",
                                                                   std::get<0>(data), std::get<1>(data),
                                                                   std::get<2>(data), std::get<3>(data));
        return mesh;
    }
    
    instanced_mesh_shared_ptr mesh_constructor::create_boxes(const glm::vec3& min_bound,
                                                             const glm::vec3& max_bound,
                                                             i32 num_instances)
    {
        std::tuple<vbo_shared_ptr, ibo_shared_ptr> box_data = mesh_constructor::create_box_data(min_bound, max_bound);
        instanced_mesh_shared_ptr mesh = instanced_mesh::construct("box", std::get<0>(box_data), std::get<1>(box_data), num_instances);
        return mesh;
    }
    
    mesh_shared_ptr mesh_constructor::create_box(const glm::vec3& min_bound,
                                                 const glm::vec3& max_bound)
    {
        std::tuple<vbo_shared_ptr, ibo_shared_ptr> box_data = mesh_constructor::create_box_data(min_bound, max_bound);
        mesh_shared_ptr mesh = mesh::construct("box", std::get<0>(box_data), std::get<1>(box_data), min_bound, max_bound);
        return mesh;
    }
    
    mesh_shared_ptr mesh_constructor::create_ocean_plane(i32 size, f32 altitude)
    {
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(9 * 4, GL_STATIC_DRAW);
        vbo::vertex_attribute *vertices = vbo->lock();
        ui32 index = 0;
        for(i32 i = -1; i <= 1; ++i)
        {
            for(i32 j = -1; j <= 1; ++j)
            {
                vertices[index * 4 + 0].m_position = glm::vec3(size * i, altitude, size * j);
                vertices[index * 4 + 1].m_position = glm::vec3(size * i + size, altitude, size * j);
                vertices[index * 4 + 2].m_position = glm::vec3(size * i + size, altitude, size * j + size);
                vertices[index * 4 + 3].m_position = glm::vec3(size * i, altitude, size * j + size);
                
                vertices[index * 4 + 0].m_extra = glm::u8vec4(i == 0 && j == 0 ? 1 : 0, 0, 0, 0);
                vertices[index * 4 + 1].m_extra = glm::u8vec4(i == 0 && j == 0 ? 1 : 0, 0, 0, 0);
                vertices[index * 4 + 2].m_extra = glm::u8vec4(i == 0 && j == 0 ? 1 : 0, 0, 0, 0);
                vertices[index * 4 + 3].m_extra = glm::u8vec4(i == 0 && j == 0 ? 1 : 0, 0, 0, 0);
                
                vertices[index * 4 + 0].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
                vertices[index * 4 + 1].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
                vertices[index * 4 + 2].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
                vertices[index * 4 + 3].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
                
                vertices[index * 4 + 0].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f, 0.f));
                vertices[index * 4 + 1].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f, 0.f));
                vertices[index * 4 + 2].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f, 1.f));
                vertices[index * 4 + 3].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f, 1.f));
                
                index++;
            }
        }
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(9 * 6, GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        index = 0;
        for(i32 i = -1; i <= 1; ++i)
        {
            for(i32 j = -1; j <= 1; ++j)
            {
                indices[index * 6 + 0] = index * 4 + 0;
                indices[index * 6 + 1] = index * 4 + 1;
                indices[index * 6 + 2] = index * 4 + 2;
                indices[index * 6 + 3] = index * 4 + 0;
                indices[index * 6 + 4] = index * 4 + 2;
                indices[index * 6 + 5] = index * 4 + 3;
                
                index++;
            }
        }
        ibo->unlock();
        mesh_shared_ptr mesh = mesh::construct("ocean_plane", vbo, ibo, glm::vec3(INT16_MIN), glm::vec3(INT16_MAX));
        return mesh;
    }
    
    mesh_shared_ptr mesh_constructor::create_grid(i32 rows, i32 columns)
    {
        i32 num_vertices = (rows + 1) * (columns + 1) * 4;
        i32 num_indices = (rows + 1) * (columns + 1) * 4;
        
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(num_vertices, GL_STATIC_DRAW);
        vbo::vertex_attribute* vertices = vbo->lock();

        i32 index = 0;
        for(i32 i = 0; i <= rows; ++i)
        {
            vertices[index++].m_position = glm::vec3(i, 0.f, 0.f);
            vertices[index++].m_position = glm::vec3(i, 0.f, columns);
        }
        
        for(i32 i = 0; i <= columns; ++i)
        {
            vertices[index++].m_position = glm::vec3(0.f, 0.f, i);
            vertices[index++].m_position = glm::vec3(rows, 0.f, i);
        }
        
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(num_indices * 4, GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        for(i32 i = 0; i < num_indices; ++i)
        {
            indices[i] = i;
        }
        ibo->unlock();
        mesh_shared_ptr mesh = gb::mesh::construct("wireframe_grid",
                                                   vbo, ibo,
                                                   glm::vec3(0.f), glm::vec3(rows, 0.f, columns),
                                                   GL_LINES);
        return mesh;
    }
    
    mesh_shared_ptr mesh_constructor::create_3d_tile_cursor()
    {
        vbo_shared_ptr vbo = std::make_shared<gb::vbo>(4, GL_STATIC_DRAW);
        vbo::vertex_attribute* vertices = vbo->lock();
        
        vertices[0].m_position = glm::vec3(0.f);
        vertices[1].m_position = glm::vec3(1.f, 0.f, 0.f);
        vertices[2].m_position = glm::vec3(1.f, 0.f, 1.f);
        vertices[3].m_position = glm::vec3(0.f, 0.f, 1.f);
        
        vertices[0].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
        vertices[1].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
        vertices[2].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
        vertices[3].m_normal = glm::packSnorm4x8(glm::vec4(0.f, 1.f, 0.f, 0.f));
        
        vertices[0].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f,  0.f));
        vertices[1].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f,  0.f));
        vertices[2].m_texcoord = glm::packUnorm2x16(glm::vec2(1.f,  1.f));
        vertices[3].m_texcoord = glm::packUnorm2x16(glm::vec2(0.f,  1.f));
        
        vertices[0].m_color = glm::u8vec4(0, 255, 0, 255);
        vertices[1].m_color = glm::u8vec4(0, 255, 0, 255);
        vertices[2].m_color = glm::u8vec4(0, 255, 0, 255);
        vertices[3].m_color = glm::u8vec4(0, 255, 0, 255);
        
        vbo->unlock();
        
        ibo_shared_ptr ibo = std::make_shared<gb::ibo>(6, GL_STATIC_DRAW);
        ui16* indices = ibo->lock();
        
        indices[0] = 0;
        indices[1] = 1;
        indices[2] = 2;
        indices[3] = 0;
        indices[4] = 2;
        indices[5] = 3;
        
        ibo->unlock();
        
        mesh_shared_ptr mesh = gb::mesh::construct("tile_cursor", vbo, ibo,
                                                   glm::vec3(0.f), glm::vec3(1.f, 0.f, 1.f));
        return mesh;

    }
    
    mesh_shared_ptr mesh_constructor::create_model3d_cursor()
    {
        return nullptr;
    }
    
    glm::vec3 mesh_constructor::generate_tangent(const glm::vec3& point_01, const glm::vec3& point_02, const glm::vec3& point_03,
                                                 const glm::vec2& texcoord_01, const glm::vec2& texcoord_02, const glm::vec2& texcoord_03)
    {
        glm::vec3 P = point_02 - point_01;
        glm::vec3 Q = point_03 - point_01;
        f32 s1 = texcoord_02.x - texcoord_01.x;
        f32 t1 = texcoord_02.y - texcoord_01.y;
        f32 s2 = texcoord_03.x - texcoord_01.x;
        f32 t2 = texcoord_03.y - texcoord_01.y;
        f32 pqMatrix[2][3];
        pqMatrix[0][0] = P[0];
        pqMatrix[0][1] = P[1];
        pqMatrix[0][2] = P[2];
        pqMatrix[1][0] = Q[0];
        pqMatrix[1][1] = Q[1];
        pqMatrix[1][2] = Q[2];
        f32 temp = 1.0f / ( s1 * t2 - s2 * t1);
        f32 stMatrix[2][2];
        stMatrix[0][0] = t2 * temp;
        stMatrix[0][1] = -t1 * temp;
        stMatrix[1][0] = -s2 * temp;
        stMatrix[1][1] = s1 * temp;
        f32 tbMatrix[2][3];
        tbMatrix[0][0] = stMatrix[0][0] * pqMatrix[0][0] + stMatrix[0][1] * pqMatrix[1][0];
        tbMatrix[0][1] = stMatrix[0][0] * pqMatrix[0][1] + stMatrix[0][1] * pqMatrix[1][1];
        tbMatrix[0][2] = stMatrix[0][0] * pqMatrix[0][2] + stMatrix[0][1] * pqMatrix[1][2];
        tbMatrix[1][0] = stMatrix[1][0] * pqMatrix[0][0] + stMatrix[1][1] * pqMatrix[1][0];
        tbMatrix[1][1] = stMatrix[1][0] * pqMatrix[0][1] + stMatrix[1][1] * pqMatrix[1][1];
        tbMatrix[1][2] = stMatrix[1][0] * pqMatrix[0][2] + stMatrix[1][1] * pqMatrix[1][2];
        return glm::normalize(glm::vec3(tbMatrix[0][0], tbMatrix[0][1], tbMatrix[0][2]));
    }
    
    glm::vec3 mesh_constructor::get_closest_point_on_line(const glm::vec3& a, const glm::vec3& b, const glm::vec3& p)
    {
        glm::vec3 c = p - a;
        glm::vec3 v = b - a;
        f32 d = v.length();
        v = glm::normalize(v);
        f32 t = glm::dot( v, c );
        
        if (t < .0f)
        {
            return a;
        }
        if ( t > d )
        {
            return b;
        }
        v *= t;
        return ( a + v );
    }
    
    glm::vec3 mesh_constructor::ortogonalize(const glm::vec3& v1, const glm::vec3& v2)
    {
        glm::vec3 v2ProjV1 = mesh_constructor::get_closest_point_on_line(v1, -v1, v2);
        glm::vec3 res = v2 - v2ProjV1;
        res = glm::normalize(res);
        return res;
    }
}