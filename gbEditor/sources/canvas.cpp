//
//  canvas.cpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/12/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "canvas.h"
#include "canvas_chunk.h"
#include "ces_material_component.h"
#include "ces_material_extension.h"
#include "mesh.h"
#include "texture.h"
#include "material.h"
#include "render_target.h"
#include "graphics_context.h"
#include "mesh_constructor.h"

namespace gb
{
    namespace ed
    {
        canvas::canvas() :
        m_descriptor(nullptr)
        {
            ces_material_component_shared_ptr material_component = std::make_shared<ces_material_component>();
            ces_entity::add_component(material_component);
            
            size.setter([=](const glm::vec2& size) {
                assert(static_cast<i32>(size.x) % static_cast<i32>(canvas_chunk::k_size) == 0);
                assert(static_cast<i32>(size.y) % static_cast<i32>(canvas_chunk::k_size) == 0);
                
                for(const auto& chunk : m_chunks)
                {
                    canvas::remove_child(chunk);
                }
                m_chunks.clear();
                m_data.clear();
                m_textures.clear();
                
                if(m_descriptor)
                {
                    m_descriptor->deallocate();
                }
                
                if(!canvas::is_data_exist("canvas"))
                {
                    canvas::create_data("canvas", size);
                }
                
                m_descriptor = std::make_shared<memory_map>();
                m_descriptor->allocate(canvas::get_data_filename("canvas"));
                
                m_size = size / canvas_chunk::k_size;
                
                m_chunks.resize(m_size.x * m_size.y);
                m_data.resize(m_size.x * m_size.y);
                m_textures.resize(m_size.x * m_size.y);
                
                i32 offset = 0;
                i32 index = 0;
                
                for(i32 i = 0; i < m_size.x; ++i)
                {
                    for(i32 j = 0; j < m_size.y; ++j)
                    {
                        index = i + j * m_size.x;
                        
                        m_data[index] = std::make_shared<canvas_chunk_texture_mmap_RGB565>(m_descriptor);
                        m_data[index]->size = canvas_chunk::k_size * canvas_chunk::k_size;
                        m_data[index]->offset = offset;
                        offset += canvas_chunk::k_size * canvas_chunk::k_size;
                        
                        ui32 texture_id;
                        gl_create_textures(1, &texture_id);
                        m_textures[index] = texture::construct("canvas", texture_id, canvas_chunk::k_size, canvas_chunk::k_size);
                        m_textures[index]->set_wrap_mode(GL_CLAMP_TO_EDGE);
                        m_textures[index]->set_mag_filter(GL_LINEAR);
                        m_textures[index]->set_min_filter(GL_LINEAR);
                        
                        m_textures[index]->bind();
                        
                        gl_texture_image2d(GL_TEXTURE_2D, 0, GL_RGB, canvas_chunk::k_size, canvas_chunk::k_size, 0, GL_RGB,
                                           GL_UNSIGNED_SHORT_5_6_5, m_data[index]->pointer);
                        
                        canvas_chunk_shared_ptr chunk = std::make_shared<canvas_chunk>();
                        for(const auto& iterator_01 : m_materials)
                        {
                            for(const auto& iterator_02 : iterator_01.second)
                            {
                                material_shared_ptr material = std::make_shared<gb::material>();
                                material->set_shader(iterator_02.second->get_shader());
                                material->set_texture(m_textures[index], e_shader_sampler_01);
                                
                                material->set_culling(false);
                                
                                material->set_stencil_test(false);
                                
                                material->set_blending(false);
                                
                                material->set_depth_test(false);
                                material->set_depth_mask(false);
                                
                                ces_material_extension::add_material(chunk, iterator_01.first, iterator_02.first, material);
                            }
                        }
                        chunk->position = glm::vec2(i * canvas_chunk::k_size, j * canvas_chunk::k_size);
                        m_chunks[index] = chunk;
                        canvas::add_child(chunk);
                    }
                }
            });
            size.getter([=]() {
                return glm::vec2(m_size.x * canvas_chunk::k_size,
                                 m_size.y * canvas_chunk::k_size);
            });
        }
        
        canvas::~canvas()
        {
            if(m_descriptor)
            {
                m_descriptor->deallocate();
            }
        }
        
        std::once_flag g_create_draw_canvas_material_once;
        material_shared_ptr canvas::get_draw_material(const texture_shared_ptr& canvas,
                                                      const texture_shared_ptr& brush)
        {
            static material_shared_ptr material = std::make_shared<gb::material>();
            std::call_once(g_create_draw_canvas_material_once, []() {
                
                shader_shared_ptr shader = shader::construct("shader_splatting_tex2d", shader_splatting_tex2d_vert, shader_splatting_tex2d_frag);
                assert(shader != nullptr);
                material->set_shader(shader);
                
                material->set_culling(false);
                
                material->set_stencil_test(false);
                
                material->set_blending(false);
                
                material->set_depth_test(false);
                material->set_depth_mask(false);
            });
            
            assert(canvas->is_loaded() && canvas->is_commited());
            material->set_texture(canvas, e_shader_sampler_01);
            
            assert(brush->is_loaded() && brush->is_commited());
            material->set_texture(brush, e_shader_sampler_02);
            
            return material;
        }
        
        void canvas::add_material(const std::string& technique_name, i32 technique_pass, const material_shared_ptr& material)
        {
            m_materials[technique_name][technique_pass] = material;
        }
        
        void canvas::remove_material(const std::string& technique_name, i32 technique_pass)
        {
            const auto& iterator_01 = m_materials.find(technique_name);
            if(iterator_01 != m_materials.end())
            {
                const auto& iterator_02 = iterator_01->second.find(technique_pass);
                if(iterator_02 != iterator_01->second.end())
                {
                    m_materials[technique_name].erase(iterator_02);
                }
            }
        }
        
        std::string canvas::get_data_filename(const std::string &filename)
        {
            std::ostringstream stringstream;
            stringstream<<filename<<".data";
            
#if defined(__IOS__)
            
            return documentspath() + stringstream.str();
            
#endif
            
            return stringstream.str();
        }
        
        bool canvas::is_data_exist(const std::string& filename)
        {
            std::ifstream stream(canvas::get_data_filename(filename));
            bool is_exist = stream.good();
            stream.close();
            return is_exist;
        }
        
        void canvas::create_data(const std::string& filename, const glm::vec2& size)
        {
            std::ofstream stream;
            stream.open(canvas::get_data_filename(filename), std::ios::binary | std::ios::out | std::ios::trunc);
            if(!stream.is_open())
            {
                assert(false);
            }
            
            ui16 color = TO_RGB565(127, 127, 127);
            for(i32 i = 0; i < size.x * size.y; ++i)
            {
                stream.write((char*)&color, sizeof(ui16));
            }
            stream.close();
        }
        
        void canvas::draw(const glm::vec2& point, const texture_shared_ptr& brush)
        {
            glm::ivec2 index = glm::ivec2(point.x / canvas_chunk::k_size, point.y / canvas_chunk::k_size);
            material_shared_ptr material = canvas::get_draw_material(m_textures[index.x + index.y * m_size.x], brush);
            
            
            glm::vec2 position = m_chunks[index.x + index.y * m_size.x]->position;
            glm::vec2 offset = (point - position) / canvas_chunk::k_size * 2.f - 1.f;
            std::cout<<"offset x: "<<offset.x<<" ,y: "<<offset.y<<std::endl;
            offset.y *= -1.f;
            material->set_custom_shader_uniform(offset, "u_point");
         
            mesh_shared_ptr screen_quad = mesh_constructor::create_screen_quad();
            render_target_shared_ptr render_target = std::make_shared<gb::render_target>(graphics_context::get_current_context(), GL_RGB, glm::ivec2(canvas_chunk::k_size));
            
            render_target->begin();
            render_target->clear();
            
            material->bind();
            
            screen_quad->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
            screen_quad->draw();
            screen_quad->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
            
            material->unbind();
            
            render_target->end();
            
            m_textures[index.x + index.y * m_size.x] = render_target->grab_color_attachment();
            unsafe_get_material_component(m_chunks[index.x + index.y * m_size.x])->set_texture(m_textures[index.x + index.y * m_size.x],
                                                                                               e_shader_sampler_01);
        }
    }
}