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
    texture_transfering_data::texture_transfering_data() :
    m_data(nullptr),
    m_width(0),
    m_height(0),
    m_texture_id(0)
    {
         m_type = e_resource_transfering_data_type_texture;
    }
    
    texture_transfering_data::~texture_transfering_data()
    {
        delete[] m_data;
    }
    
    texture::texture(const std::string& guid) :
    gb::resource(e_resource_type_texture, guid),
    m_data(nullptr),
    
    m_presetted_wrap_mode(gl::constant::repeat),
	m_presetted_mag_filter(gl::constant::nearest),
	m_presseted_min_filter(gl::constant::nearest),

    m_setted_wrap_mode(0),
    m_setted_mag_filter(0),
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
    
    texture_shared_ptr texture::construct(const std::string& guid,
                                          ui32 width,
                                          ui32 height,
                                          ui32 format,
                                          void* pixels)
    {
        std::shared_ptr<gb::texture> texture = std::make_shared<gb::texture>(guid);
        texture->m_data = std::make_shared<texture_transfering_data>();
        
        gl::command::create_textures(1, &texture->m_data->m_texture_id);
        gl::command::bind_texture(gl::constant::texture_2d, texture->m_data->m_texture_id);
        gl::command::texture_image2d(gl::constant::texture_2d, 0, format,  static_cast<i32>(width),  static_cast<i32>(height), 0, format, gl::constant::ui8_t, pixels);
        texture->m_data->m_width = width;
        texture->m_data->m_height = height;
        
#if USED_GRAPHICS_API == METAL_API
        
        texture->m_data->m_mtl_texture_id = std::make_shared<mtl_texture>(width, height, pixels, format);
        
#endif
        
        texture->m_status |= e_resource_status_loaded;
        texture->m_status |= e_resource_status_commited;
        
        return texture;
    }
    
#if USED_GRAPHICS_API == METAL_API
    
    texture_shared_ptr texture::construct(const std::string& guid,
                                          const mtl_texture_shared_ptr& mtl_texture_id,
                                          ui32 width, ui32 height)
    {
        std::shared_ptr<gb::texture> texture = std::make_shared<gb::texture>(guid);
        texture->m_data = std::make_shared<texture_transfering_data>();
        texture->m_data->m_texture_id = 0;
        texture->m_data->m_mtl_texture_id = mtl_texture_id;
        texture->m_data->m_width = width;
        texture->m_data->m_height = height;
        texture->m_status |= e_resource_status_loaded;
        texture->m_status |= e_resource_status_commited;
        return texture;
    }
    
#endif
    
    texture::~texture()
    {
        gl::command::delete_textures(1, &m_data->m_texture_id);
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
                delete[] m_data->m_data;
                m_data->m_data = nullptr;
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
    
    ui32 texture::get_width() const
    {
        return resource::is_loaded() ? m_data->m_width : 0;
    }
    
    ui32 texture::get_height() const
    {
        return resource::is_loaded() ? m_data->m_height : 0;
    }
    
    const ui8* texture::get_data() const
    {
        return resource::is_loaded() ? m_data->m_data : nullptr;
    }
    
    ui32 texture::get_texture_id() const
    {
        return m_data->m_texture_id;
    }
    
    ui32 texture::get_format() const
    {
        return resource::is_loaded() ? m_data->m_format : 0;
    }
    
    ui32 texture::get_bpp() const
    {
        return resource::is_loaded() ? m_data->m_bpp : 0;
    }
    
    ui32 texture::get_num_mips() const
    {
        return resource::is_loaded() ? m_data->m_mips : 0;
    }
    
    bool texture::is_compressed() const
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
    
    void texture::bind() const
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl::command::bind_texture(gl::constant::texture_2d, m_data->m_texture_id);
            if(m_setted_wrap_mode == 0 || m_presetted_wrap_mode != m_setted_wrap_mode)
            {
                m_setted_wrap_mode = m_presetted_wrap_mode;
                gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_wrap_s, m_setted_wrap_mode);
                gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_wrap_t, m_setted_wrap_mode);
            }
            if(m_setted_mag_filter == 0 || m_presetted_mag_filter != m_setted_mag_filter)
            {
                m_setted_mag_filter = m_presetted_mag_filter;
                gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_mag_filter, m_setted_mag_filter);
            }
            if(m_setted_min_filter == 0 || m_presseted_min_filter != m_setted_min_filter)
            {
                m_setted_min_filter = m_presseted_min_filter;
                gl::command::texture_parameter_i(gl::constant::texture_2d, gl::constant::texture_min_filter, m_setted_min_filter);
            }
        }
    }
    
    void texture::unbind() const
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            gl::command::bind_texture(gl::constant::texture_2d, NULL);
        }
    }
    
#if USED_GRAPHICS_API == METAL_API
    
    std::shared_ptr<mtl_texture> texture::get_mtl_texture_id() const
    {
        return m_data ? m_data->m_mtl_texture_id : nullptr;
    }
    
#endif
}
