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
    
    texture_commiter_png::~texture_commiter_png(void)
    {
        
    }
    
    void texture_commiter_png::commit(void)
    {
        m_status = e_commiter_status_in_progress;
        assert(m_resource != nullptr);
        std::shared_ptr<texture> texture = std::static_pointer_cast<gb::texture>(m_resource);
        
        ui32 texture_id = 0;
        gl_create_textures(1, &texture_id);
        gl_bind_texture(GL_TEXTURE_2D, texture_id);
        
        i32 width = texture->get_width();
        i32 height = texture->get_height();
        i32 format = texture->get_format();
        const ui8* data = texture->get_data();
        
        gl_texture_image2d(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, (GLvoid*)&data[0]);
        gl_generate_mipmap(GL_TEXTURE_2D);
    
        m_status = e_commiter_status_success;
        
        std::shared_ptr<texture_transfering_data> transfering_data = std::make_shared<texture_transfering_data>();
        transfering_data->m_texture_id = texture_id;
        resource_commiter::on_transfering_data_commited(transfering_data);
    }
}