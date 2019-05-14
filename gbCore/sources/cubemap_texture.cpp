//
//  cubemap_texture.cpp
//  gbCore
//
//  Created by serhii.s on 5/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "cubemap_texture.h"
#include "resource_status.h"

namespace gb
{
    cubemap_texture_transfering_data::cubemap_texture_transfering_data() :
    m_size(0),
    m_texture_id(0)
    {
        m_data.fill(nullptr);
        m_type = e_resource_transfering_data_type_cubemap_texture;
    }
    
    cubemap_texture_transfering_data::~cubemap_texture_transfering_data()
    {
        for (ui32 i = 0; i < m_data.size(); i++)
        {
            delete [] m_data[i];
        }
    }
    
    cubemap_texture::cubemap_texture(const std::string& guid) :
    gb::texture(guid)
    {
        m_type = e_resource_type_cubemap_texture;
    }
    
    cubemap_texture_shared_ptr cubemap_texture::construct(const std::string& guid,
                                                          ui32 texture_id,
                                                          ui32 size)
    {
        const auto texture = std::make_shared<gb::cubemap_texture>(guid);
        texture->m_data = std::make_shared<cubemap_texture_transfering_data>();
        texture->m_data->m_texture_id = texture_id;
        texture->m_data->m_size = size;
        texture->m_status |= e_resource_status_loaded;
        texture->m_status |= e_resource_status_commited;
        return texture;
    }
    
    cubemap_texture_shared_ptr cubemap_texture::construct(const std::string& guid,
                                                          ui32 size,
                                                          ui32 format,
                                                          std::array<ui8*, 6> pixels)
    {
        const auto texture = std::make_shared<gb::cubemap_texture>(guid);
        texture->m_data = std::make_shared<cubemap_texture_transfering_data>();
        
        texture->m_data->m_size = size;
        
#if USED_GRAPHICS_API == METAL_API
        
        texture->m_data->m_mtl_texture_id = std::make_shared<mtl_texture>(size, pixels, format);
        
#endif
        
        texture->m_status |= e_resource_status_loaded;
        texture->m_status |= e_resource_status_commited;
        
        return texture;
    }
    
#if USED_GRAPHICS_API == METAL_API
    
    cubemap_texture_shared_ptr cubemap_texture::construct(const std::string& guid,
                                                          const mtl_texture_shared_ptr& mtl_texture_id,
                                                          ui32 size)
    {
        const auto texture = std::make_shared<gb::cubemap_texture>(guid);
        texture->m_data = std::make_shared<cubemap_texture_transfering_data>();
        texture->m_data->m_texture_id = 0;
        texture->m_data->m_mtl_texture_id = mtl_texture_id;
        texture->m_data->m_size = size;
        texture->m_status |= e_resource_status_loaded;
        texture->m_status |= e_resource_status_commited;
        return texture;
    }
    
#endif
    
    cubemap_texture::~cubemap_texture()
    {
        gl::command::delete_textures(1, &m_data->m_texture_id);
    }
    
    void cubemap_texture::on_transfering_data_serialized(const std::shared_ptr<resource_transfering_data> &data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_cubemap_texture:
            {
                m_data = std::static_pointer_cast<cubemap_texture_transfering_data>(data);
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
    
    void cubemap_texture::on_transfering_data_commited(const std::shared_ptr<resource_transfering_data> &data)
    {
        assert(data != nullptr);
        switch(data->get_type())
        {
            case e_resource_transfering_data_type_cubemap_texture:
            {
                m_data->m_texture_id = std::static_pointer_cast<cubemap_texture_transfering_data>(data)->m_texture_id;
                for (ui32 i = 0; i < m_data->m_data.size(); i++)
                {
                    delete [] m_data->m_data[i];
                }
                m_data->m_data.fill(nullptr);
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
    
    ui32 cubemap_texture::get_size() const
    {
        return resource::is_loaded() ? m_data->m_size : 0;
    }
    
    
    const ui8* cubemap_texture::get_data(ui32 slice) const
    {
        return resource::is_loaded() ? m_data->m_data.at(slice) : nullptr;
    }
    
    ui32 cubemap_texture::get_texture_id() const
    {
        return m_data->m_texture_id;
    }
    
    ui32 cubemap_texture::get_format() const
    {
        return resource::is_loaded() ? m_data->m_format : 0;
    }
    
    ui32 cubemap_texture::get_bpp() const
    {
        return resource::is_loaded() ? m_data->m_bpp : 0;
    }
    
    ui32 cubemap_texture::get_num_mips() const
    {
        return resource::is_loaded() ? m_data->m_mips : 0;
    }
    
    bool cubemap_texture::is_compressed() const
    {
        return resource::is_loaded() ? m_data->m_compressed : false;
    }
    
    void cubemap_texture::set_wrap_mode(ui32 wrap_mode)
    {

    }
    
    void cubemap_texture::set_mag_filter(ui32 mag_filter)
    {

    }
    
    void cubemap_texture::set_min_filter(ui32 min_filter)
    {

    }
    
    void cubemap_texture::bind() const
    {
        
    }
    
    void cubemap_texture::unbind() const
    {
        
    }
    
#if USED_GRAPHICS_API == METAL_API
    
    std::shared_ptr<mtl_texture> cubemap_texture::get_mtl_texture_id() const
    {
        return m_data ? m_data->m_mtl_texture_id : nullptr;
    }
    
#endif
}
