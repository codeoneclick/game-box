//
//  texture_commiter_png.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/18/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "texture_commiter_png.h"
#include "texture.h"

namespace gb
{
    texture_commiter_png::texture_commiter_png(const std::string& guid, const resource_shared_ptr& resource) :
    gb::resource_commiter(guid, resource)
    {
        
    }
    
    texture_commiter_png::~texture_commiter_png()
    {
        
    }
    
    void texture_commiter_png::commit(const resource_transfering_data_shared_ptr& transfering_data)
    {
        m_status = e_commiter_status_in_progress;
        assert(m_resource != nullptr);
        
        ui32 texture_id = 0;
        gl_create_textures(1, &texture_id);
        gl_bind_texture(GL_TEXTURE_2D, texture_id);
        
        texture_transfering_data_shared_ptr texture_transfering_data = std::static_pointer_cast<gb::texture_transfering_data>(transfering_data);
        
        gl_texture_image2d(GL_TEXTURE_2D, 0, texture_transfering_data->m_format,
                           texture_transfering_data->m_width, texture_transfering_data->m_height,
                           0, texture_transfering_data->m_format, GL_UNSIGNED_BYTE, (GLvoid*)&texture_transfering_data->m_data[0]);
        gl_generate_mipmap(GL_TEXTURE_2D);
    
        m_status = e_commiter_status_success;
        
        texture_transfering_data->m_texture_id = texture_id;
        resource_commiter::on_transfering_data_commited(texture_transfering_data);
    }
}