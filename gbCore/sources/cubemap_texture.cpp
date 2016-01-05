//
//  cubemap_texture.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/13/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "cubemap_texture.h"
#include "resource_status.h"

namespace gb
{
    cubemap_texture::cubemap_texture(const std::string& guid,
                                     const texture_shared_ptr& xpositive, const texture_shared_ptr& xnegative,
                                     const texture_shared_ptr& ypositive, const texture_shared_ptr& ynegative,
                                     const texture_shared_ptr& zpositive, const texture_shared_ptr& znegative) :
    gb::texture(guid),
    m_xpositive(xpositive),
    m_xnegative(xnegative),
    m_ypositive(ypositive),
    m_ynegative(ynegative),
    m_zpositive(zpositive),
    m_znegative(znegative)
    {
        m_data = std::make_shared<texture_transfering_data>();
        gl_create_textures(1, &m_data->m_texture_id);
        gl_bind_texture(GL_TEXTURE_CUBE_MAP, m_data->m_texture_id);
        gl_texture_parameter_i(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        gl_texture_parameter_i(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        gl_texture_parameter_i(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        gl_texture_parameter_i(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
    
    cubemap_texture_shared_ptr cubemap_texture::construct(const std::string& guid,
                                                          const texture_shared_ptr& xpositive, const texture_shared_ptr& xnegative,
                                                          const texture_shared_ptr& ypositive, const texture_shared_ptr& ynegative,
                                                          const texture_shared_ptr& zpositive, const texture_shared_ptr& znegative)
    {
        cubemap_texture_shared_ptr texture = std::make_shared<cubemap_texture>(guid,
                                                                               xpositive, xnegative,
                                                                               ypositive, ynegative,
                                                                               zpositive, znegative);
        return texture;
    }
    
    cubemap_texture::~cubemap_texture()
    {
        
    }
    
    void cubemap_texture::on_resource_loaded(const resource_shared_ptr& resource, bool success)
    {
        gl_bind_texture(GL_TEXTURE_CUBE_MAP, m_data->m_texture_id);
        
        texture_shared_ptr texture = std::static_pointer_cast<gb::texture>(resource);
        GLenum face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        
        if(resource == m_xpositive)
        {
            face = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
        }
        else if(resource == m_xnegative)
        {
            face = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
        }
        else if(resource == m_ypositive)
        {
            face = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
        }
        else if(resource == m_ynegative)
        {
            face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
        }
        else if(resource == m_zpositive)
        {
            face = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
        }
        else if(resource == m_znegative)
        {
            face = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;
        }
        else
        {
            assert(false);
        }
        
        i32 width = texture->get_width();
        i32 height = texture->get_height();
        i32 num_mips = texture->get_num_mips();
        const ui8* data = texture->get_data();
        
        assert(data != nullptr);
        
        if(num_mips != 0)
        {
            for (ui32 mip = 0; mip < num_mips && width > 0 && height > 0; ++mip)
            {
                GLsizei size = std::max(32, static_cast<i32>(width) * static_cast<i32>(height) * texture->get_format() == GL_RGBA ? 4 : 3);
                texture->is_compressed() ?
                gl_compressed_texture_image2d(face, mip, texture->get_format(), width, height, 0, size, data) :
                gl_texture_image2d(face, mip, texture->get_format(), width, height, 0, texture->get_format(), GL_UNSIGNED_BYTE, data);
                data += size;
                width >>= 1; height >>= 1;
            }
        }
        else
        {
            GLsizei size = std::max(32, static_cast<i32>(width) * static_cast<i32>(height) * texture->get_format() == GL_RGBA ? 4 : 3);
            texture->is_compressed() ?
            gl_compressed_texture_image2d(face, 0, texture->get_format(), width, height, 0, size, data) :
            gl_texture_image2d(face, 0, texture->get_format(), width, height, 0, texture->get_format(), GL_UNSIGNED_BYTE, data);
        }
        
        if(m_xpositive->is_commited() &&
           m_xnegative->is_commited() &&
           m_ypositive->is_commited() &&
           m_ynegative->is_commited() &&
           m_zpositive->is_commited() &&
           m_znegative->is_commited())
        {
            m_status |= e_resource_status_loaded;
            m_status |= e_resource_status_commited;
        }
    }
    
    void  cubemap_texture::bind() const
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_bind_texture(GL_TEXTURE_CUBE_MAP, m_data->m_texture_id);
        }
    }
    
    void  cubemap_texture::unbind() const
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_bind_texture(GL_TEXTURE_CUBE_MAP, NULL);
        }
    }
}