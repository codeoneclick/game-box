//
//  texture.cpp
//  gbCore
//
//  Created by Sergey Sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "texture.h"
#include "resource_status.h"

namespace gb
{
    texture_transfering_data::texture_transfering_data(void) :
    m_data(nullptr),
    m_width(0),
    m_height(0),
    m_texture_id(0)
    {
         m_type = e_resource_transfering_data_type_texture;
    }
    
    texture_transfering_data::~texture_transfering_data(void)
    {
        delete[] m_data;
    }
    
    texture::texture(const std::string& guid) :
    gb::resource(e_resource_type_texture, guid),
    m_data(nullptr),
    m_presetted_wrap_mode(GL_REPEAT),
    m_setted_wrap_mode(0),
    m_presetted_mag_filter(GL_NEAREST),
    m_setted_mag_filter(0),
    m_presseted_min_filter(GL_NEAREST),
    m_setted_min_filter(0)
    {
        
    }
    
    std::shared_ptr<texture> texture::construct(const std::string& guid,
                                                ui32 texture_id,
                                                ui32 width,
                                                ui32 height)
    {
        std::shared_ptr<gb::texture> texture = std::make_shared<gb::texture>(guid);
        texture->m_data = std::make_shared<texture_transfering_data>();
        texture->m_data->m_texture_id = texture_id;
        texture->m_data->m_width = width;
        texture->m_data->m_height = height;
        texture->m_status |= e_resource_status_loaded;
        texture->m_status |= e_resource_status_commited;
        return texture;
    }
    
    texture::~texture(void)
    {
        gl_delete_textures(1, &m_data->m_texture_id);
    }
    
    void texture::on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data> &data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_texture:
            {
                m_data = std::static_pointer_cast<texture_transfering_data>(data);
                m_status |= e_resource_status_loaded;
            }
                break;
            default:
            {
                assert(false);
            }
                break;
        }
    }
    
    void texture::on_transfering_data_commited(const std::shared_ptr<resource_transfering_data> &data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_texture:
            {
                m_data->m_texture_id = std::static_pointer_cast<texture_transfering_data>(data)->m_texture_id;
                assert(m_data->m_texture_id != 0);
                
                /* delete[] m_data->m_data;
                   m_data->m_data = nullptr; */
                
                m_status |= e_resource_status_commited;
            }
                break;
            default:
            {
                assert(false);
            }
                break;
        }
    }
    
    ui32 texture::get_width(void) const
    {
        return resource::is_loaded() ? m_data->m_width : 0;
    }
    
    ui32 texture::get_height(void) const
    {
        return resource::is_loaded() ? m_data->m_height : 0;
    }
    
    const ui8* texture::get_data(void) const
    {
        return resource::is_loaded() ? m_data->m_data : nullptr;
    }
    
    ui32 texture::get_texture_id(void) const
    {
        return m_data->m_texture_id;
    }
    
    GLenum texture::get_format(void) const
    {
        return resource::is_loaded() ? m_data->m_format : 0;
    }
    
    ui32 texture::get_bpp(void) const
    {
        return resource::is_loaded() ? m_data->m_bpp : 0;
    }
    
    ui32 texture::get_num_mips(void) const
    {
        return resource::is_loaded() ? m_data->m_mips : 0;
    }
    
    bool texture::is_compressed(void) const
    {
        return resource::is_loaded() ? m_data->m_compressed : false;
    }
    
    void texture::set_wrap_mode(ui32 wrap_mode)
    {
        m_presetted_wrap_mode = wrap_mode;
    }
    
    void texture::set_mag_filter(ui32 mag_filter)
    {
        m_presetted_mag_filter = mag_filter;
    }
    
    void texture::set_min_filter(ui32 min_filter)
    {
        m_presseted_min_filter = min_filter;
    }
    
    void texture::bind(void) const
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_bind_texture(GL_TEXTURE_2D, m_data->m_texture_id);
            if(m_setted_wrap_mode == 0 || m_presetted_wrap_mode != m_setted_wrap_mode)
            {
                m_setted_wrap_mode = m_presetted_wrap_mode;
                gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, m_setted_wrap_mode);
                gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, m_setted_wrap_mode);
            }
            if(m_setted_mag_filter == 0 || m_presetted_mag_filter != m_setted_mag_filter)
            {
                m_setted_mag_filter = m_presetted_mag_filter;
                gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, m_setted_mag_filter);
            }
            if(m_setted_min_filter == 0 || m_presseted_min_filter != m_setted_min_filter)
            {
                m_setted_min_filter = m_presseted_min_filter;
                gl_texture_parameter_i(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, m_setted_min_filter);
            }
        }
    }
    
    void texture::unbind(void) const
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl_bind_texture(GL_TEXTURE_2D, NULL);
        }
    }
}