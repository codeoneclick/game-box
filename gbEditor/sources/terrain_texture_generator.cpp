//
//  terrain_texture_generator.cpp
//  gbEditor
//
//  Created by sergey.sergeev on 1/29/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "terrain_texture_generator.h"
#include "texture.h"
#include "material.h"
#include "render_target.h"
#include "built_in_shaders.h"
#include "shader.h"
#include "mesh.h"
#include "mesh_constructor.h"
#include "graphics_context.h"

namespace gb
{
    namespace ed
    {
        terrain_texture_generator::terrain_texture_generator()
        {
            
        }
        
        terrain_texture_generator::~terrain_texture_generator()
        {
            
        }
        
        std::once_flag g_create_splatting_material_once;
        material_shared_ptr terrain_texture_generator::get_splatting_material(const std::vector<texture_shared_ptr>& textures,
                                                                              const texture_shared_ptr& mask)
        {
            static material_shared_ptr material = std::make_shared<gb::material>();
            std::call_once(g_create_splatting_material_once, []() {
                
                shader_shared_ptr shader = shader::construct("shader_splatting_tex2d", shader_splatting_tex2d_vert, shader_splatting_tex2d_frag);
                assert(shader != nullptr);
                material->set_shader(shader);
                
                material->set_culling(false);
                material->set_culling_mode(GL_BACK);
                
                material->set_blending(false);
                material->set_blending_function_source(GL_ONE);
                material->set_blending_function_destination(GL_ONE);
                
                material->set_depth_test(false);
                material->set_depth_mask(true);
            });
            
            assert(textures[0]->is_loaded() && textures[0]->is_commited());
            material->set_texture(textures[0], e_shader_sampler_01);
            assert(textures[1]->is_loaded() && textures[1]->is_commited());
            material->set_texture(textures[1], e_shader_sampler_02);
            assert(textures[2]->is_loaded() && textures[2]->is_commited());
            material->set_texture(textures[2], e_shader_sampler_03);
            
            assert(mask->is_loaded() && mask->is_commited());
            material->set_texture(mask, e_shader_sampler_04);
            
            return material;
        }
        
        texture_shared_ptr terrain_texture_generator::create_splatting_mask_texture(i32 width, i32 height)
        {
            ui16* pixels = new ui16[width * height];
            std::memset(pixels, TO_RGB565(255, 0, 0), width * height);
            
            ui32 texture_id;
            gl_create_textures(1, &texture_id);
            
            texture_shared_ptr texture = texture::construct("splatting_mask", texture_id, width, height);
            texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
            texture->set_mag_filter(GL_LINEAR);
            texture->set_min_filter(GL_LINEAR);
            
            texture->bind();
            
            gl_texture_image2d(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, pixels);
            
            delete [] pixels;
            
            return texture;
        }
        
        texture_shared_ptr terrain_texture_generator::create_splatting_diffuse_texture(i32 width, i32 height,
                                                                                       const texture_shared_ptr& mask_texture,
                                                                                       const std::vector<texture_shared_ptr>& diffuse_textures)
        {
            material_shared_ptr material = terrain_texture_generator::get_splatting_material(diffuse_textures, mask_texture);
            mesh_shared_ptr screen_quad = mesh_constructor::create_screen_quad();
            
            render_target_shared_ptr render_target = std::make_shared<gb::render_target>(graphics_context::get_current_context(), GL_RGBA, width, height);
            
            i32 size = width * height * 4;
            ui8 *data = new ui8[size];
            
            render_target->begin();
            render_target->clear();
            
            material->bind();

            screen_quad->bind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
            screen_quad->draw();
            screen_quad->unbind(material->get_shader()->get_guid(), material->get_shader()->get_attributes());
            
            material->unbind();
            
            render_target->end(data);
            
            ui32 size_565 = size / 4;
            ui16* data_565 = new ui16[size_565];
            ui32 index = 0;
            for(ui32 i = 0; i < size; i += 4)
            {
                data_565[index] = TO_RGB565(data[i + 0], data[i + 1], data[i + 2]);
                index++;
            }

            
            ui32 texture_id;
            gl_create_textures(1, &texture_id);
            
            texture_shared_ptr texture = texture::construct("splatting_mask", texture_id, width, height);
            texture->set_wrap_mode(GL_CLAMP_TO_EDGE);
            texture->set_mag_filter(GL_LINEAR);
            texture->set_min_filter(GL_LINEAR);
            
            texture->bind();
            
            gl_texture_image2d(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_SHORT_5_6_5, data_565);
            
            delete [] data_565;
            delete [] data;
            
            return texture;
        }
    }
}