//
//  terrain.cpp
//  gbEditor
//
//  Created by sergey.sergeev on 1/29/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "terrain.h"
#include "ces_geometry_quad_component.h"
#include "ces_material_component.h"
#include "ces_transformation_component.h"
#include "terrain_texture_generator.h"
#include "glm_extensions.h"

namespace gb
{
    namespace ed
    {
        terrain::terrain() :
        m_mask_texture(nullptr)
        {
            m_diffuse_textures.resize(3, nullptr);
            m_normalmap_textures.resize(3, nullptr);
            
            m_texture_generator = std::make_shared<terrain_texture_generator>();
        }
        
        terrain::~terrain()
        {
            
        }
        
        void terrain::set_mask_texture(const texture_shared_ptr& texture)
        {
            m_mask_texture = texture;
        }
        
        void terrain::set_diffuse_textures(const std::vector<texture_shared_ptr>& textures)
        {
            assert(textures.size() == 3);
            for(i32 i = 0; i < 3; ++i)
            {
                m_diffuse_textures[i] = textures[i];
            }
        }
        
        void terrain::set_normalmap_textures(const std::vector<texture_shared_ptr>& textures)
        {
            assert(textures.size() == 3);
            for(i32 i = 0; i < 3; ++i)
            {
                m_normalmap_textures[i] = textures[i];
            }
        }
        
        void terrain::generate()
        {
            if(!m_mask_texture)
            {
                m_mask_texture = m_texture_generator->create_splatting_mask_texture(sprite::get_size().x, sprite::get_size().y);
            }
            texture_shared_ptr texture = m_texture_generator->create_splatting_diffuse_texture(sprite::get_size().x, sprite::get_size().y, m_mask_texture,
                                                                                               m_diffuse_textures);
            unsafe_get_material_component_from_this->set_texture(texture, e_shader_sampler_01);
        }
    }
}