//
//  landscape.cpp
//  gbEditor
//
//  Created by Serhii Serhiiv on 2/1/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "landscape.h"
#include "shader.h"
#include "texture.h"
#include "material.h"
#include "mesh.h"
#include "render_target.h"
#include "graphics_context.h"
#include "mesh_constructor.h"
#include "ces_material_component.h"

namespace gb
{
    namespace ed
    {
        landscape::landscape()
        {
            m_masks.resize(1, nullptr);
        }
        
        landscape::~landscape()
        {
            
        }
        
        std::once_flag g_create_main_splatting_material_once;
        material_shared_ptr landscape::get_main_splatting_material(const texture_shared_ptr& layer_1_brush,
                                                                   const texture_shared_ptr& layer_2_brush,
                                                                   const texture_shared_ptr& layer_3_brush,
                                                                   const texture_shared_ptr& mask)
        {
            static material_shared_ptr material = std::make_shared<gb::material>();
            std::call_once(g_create_main_splatting_material_once, []() {
                
                shader_shared_ptr shader = shader::construct("shader_splatting_tex2d", shader_splatting_tex2d_vert, shader_splatting_tex2d_frag);
                assert(shader != nullptr);
                material->set_shader(shader);
                
                material->set_culling(false);
                
                material->set_stencil_test(false);
                
                material->set_blending(false);
                
                material->set_depth_test(false);
                material->set_depth_mask(false);
            });
            
            assert(layer_1_brush->is_loaded() && layer_1_brush->is_commited());
            material->set_texture(layer_1_brush, e_shader_sampler_01);
            assert(layer_2_brush->is_loaded() && layer_2_brush->is_commited());
            material->set_texture(layer_2_brush, e_shader_sampler_02);
            assert(layer_3_brush->is_loaded() && layer_3_brush->is_commited());
            material->set_texture(layer_3_brush, e_shader_sampler_03);
            
            assert(mask->is_loaded() && mask->is_commited());
            material->set_texture(mask, e_shader_sampler_04);
            
            return material;
        }
        
        material_shared_ptr landscape::get_additional_splatting_material(const texture_shared_ptr& layer_1_brush,
                                                                         const texture_shared_ptr& layer_2_brush,
                                                                         const texture_shared_ptr& layer_3_brush,
                                                                         const texture_shared_ptr& mask)
        {
            return nullptr;
        }

        
        void landscape::add_mask(const texture_shared_ptr& mask)
        {
            if(!m_masks[0])
            {
                m_masks[0] = mask;
            }
            else
            {
                m_masks.push_back(mask);
            }
        }
        
        void landscape::add_brush(const texture_shared_ptr& diffuse, const texture_shared_ptr& normalmap, i32 layer, i32 sampler)
        {
            if(layer == 0)
            {
                if(sampler < k_brushes_count_on_main_layer)
                {
                    m_main_layer_brushes[sampler] = std::make_pair(diffuse, normalmap);
                }
                else
                {
                    assert(false);
                }
            }
            else
            {
                layer--;
                
                if(layer <= m_additional_layer_brushes.size())
                {
                    if(layer == m_additional_layer_brushes.size())
                    {
                        m_additional_layer_brushes.push_back(std::array<std::pair<texture_shared_ptr, texture_shared_ptr>, k_brushes_count_on_additional_layer>());
                    }
                    if(sampler < k_brushes_count_on_main_layer)
                    {
                        m_additional_layer_brushes[layer][sampler] = std::make_pair(diffuse, normalmap);
                    }
                    else
                    {
                        assert(false);
                    }
                }
                else
                {
                    assert(false);
                }
            }
        }
        
        texture_shared_ptr landscape::create_splatting_mask(const glm::vec2& size)
        {
            ui16* data = new ui16[static_cast<i32>(size.x) * static_cast<i32>(size.y)];
            
            for(ui32 i = 0; i < size.x; i++)
            {
                for(ui32 j = 0; j < size.y; ++j)
                {
                    data[i + j * static_cast<i32>(size.x)] = TO_RGB565(255, 0, 0);
                }
            }
            
            ui32 texture_id;
            gl_create_textures(1, &texture_id);
            
            texture_shared_ptr texture = texture::construct("splatting_mask", texture_id, size.x, size.y);
            texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
            texture->set_mag_filter(GL_LINEAR);
            texture->set_min_filter(GL_LINEAR);
            
            texture->bind();
            
            gl_texture_image2d(GL_TEXTURE_2D, 0, GL_RGB, size.x, size.y, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data);
            
            delete [] data;
            
            return texture;
        }
        
        texture_shared_ptr landscape::get_brush(i32 layer, i32 sampler)
        {
            texture_shared_ptr brush = nullptr;
            if(layer == 0 && sampler < k_brushes_count_on_main_layer)
            {
                brush = std::get<0>(m_main_layer_brushes[sampler]);
            }
            else if((layer - 1) < m_additional_layer_brushes.size() && sampler < k_brushes_count_on_additional_layer)
            {
                layer--;
                brush = std::get<0>(m_additional_layer_brushes[layer][sampler]);
            }
            return brush;
        }
        
        void landscape::generate()
        {
            /*if(m_masks[0] == nullptr)
            {
                m_masks[0] = landscape::create_splatting_mask(sprite::size);
            }
            material_shared_ptr material = landscape::get_main_splatting_material(std::get<0>(m_main_layer_brushes[0]),
                                                                                  std::get<0>(m_main_layer_brushes[1]),
                                                                                  std::get<0>(m_main_layer_brushes[2]),
                                                                                  m_masks[0]);
            mesh_shared_ptr screen_quad = mesh_constructor::create_screen_quad();
            glm::vec2 size = sprite::size;
            render_target_shared_ptr render_target = std::make_shared<gb::render_target>(graphics_context::get_current_context(), GL_RGBA, size.x, size.y);
            
            render_target->begin();
            render_target->clear();
            
            material->bind();
            
            screen_quad->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
            screen_quad->draw();
            screen_quad->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
            
            material->unbind();
            
            render_target->end();
            
            unsafe_get_material_component_from_this->set_texture(render_target->grab_color_attachment(), e_shader_sampler_01);*/
        }
    }
}