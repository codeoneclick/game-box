//
//  mesh_constructor.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "mesh_constructor.h"
#include "mesh_2d.h"
#include "mesh_3d.h"
#include "vbo.h"
#include "ibo.h"
#include "std_extensions.h"

namespace gb
{
    std::once_flag g_screen_quad_created;
    std::once_flag g_shape_quad_created;
    
    mesh_2d_shared_ptr mesh_constructor::create_screen_quad()
    {
        static mesh_2d_shared_ptr mesh = nullptr;
        std::call_once(g_screen_quad_created, [] {
            
            std::shared_ptr<vbo::vertex_declaration_PTC> vertex_declaration = std::make_shared<vbo::vertex_declaration_PTC>(4);
			vbo_shared_ptr vbo = std::make_shared<gb::vbo>(vertex_declaration, gl::constant::static_draw);

            vbo::vertex_attribute_PTC *vertices = vbo->lock<vbo::vertex_attribute_PTC>();
            
            vertices[0].m_position = glm::vec3(-1.f, -1.f, 0.f);
            vertices[0].m_texcoord = glm::vec2(0.f, 1.f);
            vertices[1].m_position = glm::vec3(-1.f, 1.f, 0.f);
            vertices[1].m_texcoord = glm::vec2(0.f, 0.f);
            vertices[2].m_position = glm::vec3(1.f, -1.f, 0.f);
            vertices[2].m_texcoord = glm::vec2(1.f, 1.f);
            vertices[3].m_position = glm::vec3(1.f, 1.f, 0.f);
            vertices[3].m_texcoord = glm::vec2(1.f, 0.f);
            vbo->unlock();
            
			ibo_shared_ptr ibo = std::make_shared<gb::ibo>(6, gl::constant::static_draw);

            ui16* indices = ibo->lock();
            indices[0] = 0;
            indices[1] = 2;
            indices[2] = 1;
            indices[3] = 1;
            indices[4] = 2;
            indices[5] = 3;
            ibo->unlock();
            
            mesh = std::make_shared<gb::mesh_2d>(vbo, ibo);
        });
        return mesh;
    }
    
    mesh_2d_shared_ptr mesh_constructor::create_shape_quad(bool is_use_batch)
    {
        std::shared_ptr<vbo::vertex_declaration_PTC> vertex_declaration = std::make_shared<vbo::vertex_declaration_PTC>(4);
		vbo_shared_ptr vbo = nullptr;

#if USED_GRAPHICS_API != NO_GRAPHICS_API

		vbo = std::make_shared<gb::vbo>(vertex_declaration, gl::constant::static_draw, is_use_batch);

#else
		vbo = std::make_shared<gb::vbo>(vertex_declaration, 0, is_use_batch);

#endif

        vbo::vertex_attribute_PTC *vertices = vbo->lock<vbo::vertex_attribute_PTC>();
        
        vertices[0].m_position = glm::vec3(-.5f, -.5f, 0.f);
        vertices[0].m_texcoord = glm::vec2(0.f, 0.f);
        vertices[0].m_color = glm::u8vec4(255);
        vertices[1].m_position = glm::vec3(-.5f, .5f, 0.f);
        vertices[1].m_texcoord = glm::vec2(0.f, 1.f);
        vertices[1].m_color = glm::u8vec4(255);
        vertices[2].m_position = glm::vec3(.5f, -.5f, 0.f);
        vertices[2].m_texcoord = glm::vec2(1.f, 0.f);
        vertices[2].m_color = glm::u8vec4(255);
        vertices[3].m_position = glm::vec3(.5f, .5f, 0.f);
        vertices[3].m_texcoord = glm::vec2(1.f, 1.f);
        vertices[3].m_color = glm::u8vec4(255);
        vbo->unlock();
        
		ibo_shared_ptr ibo = nullptr;

#if USED_GRAPHICS_API != NO_GRAPHICS_API

		ibo = std::make_shared<gb::ibo>(6, gl::constant::static_draw, is_use_batch);

#else

		ibo = std::make_shared<gb::ibo>(6, 0, is_use_batch);

#endif

        ui16* indices = ibo->lock();
        indices[0] = 0;
        indices[1] = 2;
        indices[2] = 1;
        indices[3] = 1;
        indices[4] = 2;
        indices[5] = 3;
        ibo->unlock();
        
        mesh_2d_shared_ptr mesh = std::make_shared<gb::mesh_2d>(vbo, ibo);
        return mesh;
    }
    
    mesh_2d_shared_ptr mesh_constructor::create_circle()
    {
        i32 num_subdivisions = 32;
        f32 radius = 1.f;
        
        i32 num_vertices = num_subdivisions + 1;
        
        std::shared_ptr<vbo::vertex_declaration_PTC> vertex_declaration = std::make_shared<vbo::vertex_declaration_PTC>(num_vertices);
		vbo_shared_ptr vbo = nullptr;

#if USED_GRAPHICS_API != NO_GRAPHICS_API

		vbo = std::make_shared<gb::vbo>(vertex_declaration, gl::constant::static_draw);

#else
        
		vbo = std::make_shared<gb::vbo>(vertex_declaration, 0);

#endif

        vbo::vertex_attribute_PTC *vertices = vbo->lock<vbo::vertex_attribute_PTC>();
        
        vertices[0].m_position = glm::vec3(0.f);
        
        i32 index = 1;
        for(f32 angle=0; angle <= M_PI * 2.f; angle += ((M_PI * 2) / num_subdivisions))
        {
            vertices[index++].m_position = glm::vec3(radius * cosf(angle),
                                                     radius * sinf(angle),
                                                     0.f);
        }
        vbo->unlock();
        
        index = 1;
        i32 num_indices = (num_subdivisions + 1) * 3;
		ibo_shared_ptr ibo = nullptr;

#if USED_GRAPHICS_API != NO_GRAPHICS_API

		ibo = std::make_shared<gb::ibo>(num_indices, gl::constant::static_draw);

#else

		ibo = std::make_shared<gb::ibo>(num_indices, 0);

#endif

        ui16* indices = ibo->lock();
        for (i32 i = 0; i < num_subdivisions * 3; i += 3)
        {
            indices[i + 0] = 0;
            indices[i + 1] = index++;
            if(index >= num_vertices)
            {
                break;
            }
            indices[i + 2] = index;
        }
        
        indices[num_indices - 3] = 0;
        indices[num_indices - 2] = index - 1;
        indices[num_indices - 1] = 1;
        ibo->unlock();
        
        mesh_2d_shared_ptr mesh = std::make_shared<gb::mesh_2d>(vbo, ibo);
        return mesh;
    }
    
    mesh_3d_shared_ptr mesh_constructor::create_plane_3d()
    {
        std::shared_ptr<vbo::vertex_declaration_PTC> vertex_declaration = std::make_shared<vbo::vertex_declaration_PTC>(4);
        vbo_shared_ptr vbo = nullptr;
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        vbo = std::make_shared<gb::vbo>(vertex_declaration, gl::constant::static_draw, true);
        
#else
        vbo = std::make_shared<gb::vbo>(vertex_declaration, 0, true);
        
#endif
        
        vbo::vertex_attribute_PTC *vertices = vbo->lock<vbo::vertex_attribute_PTC>();
        
        vertices[0].m_position = glm::vec3(-.5f, -.5f, 0.f);
        vertices[0].m_texcoord = glm::vec2(0.f, 0.f);
        vertices[0].m_color = glm::u8vec4(255);
        vertices[1].m_position = glm::vec3(-.5f, .5f, 0.f);
        vertices[1].m_texcoord = glm::vec2(0.f, 1.f);
        vertices[1].m_color = glm::u8vec4(255);
        vertices[2].m_position = glm::vec3(.5f, -.5f, 0.f);
        vertices[2].m_texcoord = glm::vec2(1.f, 0.f);
        vertices[2].m_color = glm::u8vec4(255);
        vertices[3].m_position = glm::vec3(.5f, .5f, 0.f);
        vertices[3].m_texcoord = glm::vec2(1.f, 1.f);
        vertices[3].m_color = glm::u8vec4(255);
        vbo->unlock();
        
        ibo_shared_ptr ibo = nullptr;
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        ibo = std::make_shared<gb::ibo>(6, gl::constant::static_draw, true);
        
#else
        
        ibo = std::make_shared<gb::ibo>(6, 0, is_use_batch);
        
#endif
        
        ui16* indices = ibo->lock();
        indices[0] = 0;
        indices[1] = 2;
        indices[2] = 1;
        indices[3] = 1;
        indices[4] = 2;
        indices[5] = 3;
        ibo->unlock();
        
        mesh_3d_shared_ptr mesh = gb::mesh_3d::construct("primitive.plane.3d", vbo, ibo);
        return mesh;
    }
    
    mesh_3d_shared_ptr mesh_constructor::create_box_3d()
    {
        return nullptr;
    }
    
    mesh_3d_shared_ptr mesh_constructor::create_sphere()
    {
        const auto sectors = 16;
        const auto rings = 16;
        const auto radius = 1.f;
        std::shared_ptr<vbo::vertex_declaration_PTNTC> vertex_declaration = std::make_shared<vbo::vertex_declaration_PTNTC>((rings + 1) * (sectors + 1));
        vbo_shared_ptr vbo = nullptr;
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        vbo = std::make_shared<gb::vbo>(vertex_declaration, gl::constant::static_draw);
        
#else
        vbo = std::make_shared<gb::vbo>(vertex_declaration, 0);
        
#endif
        
        ibo_shared_ptr ibo = nullptr;
        
#if USED_GRAPHICS_API != NO_GRAPHICS_API
        
        ibo = std::make_shared<gb::ibo>((rings * sectors + rings) * 6, gl::constant::static_draw);
        
#else
        
        ibo = std::make_shared<gb::ibo>((rings * sectors + rings) * 6, 0);
        
#endif
        
        vbo::vertex_attribute_PTNTC *vertices = vbo->lock<vbo::vertex_attribute_PTNTC>();
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
                vertices[index].m_texcoord = glm::vec2(u, v);
                
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
        
        mesh_3d_shared_ptr mesh = gb::mesh_3d::construct("primitive.sphere.3d", vbo, ibo);
        
        return mesh;
    }
}
