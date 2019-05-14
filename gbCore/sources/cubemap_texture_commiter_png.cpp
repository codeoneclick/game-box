//
//  cubemap_texture_commiter_png.cpp
//  gbCore
//
//  Created by serhii.s on 5/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "cubemap_texture_commiter_png.h"
#include "cubemap_texture.h"

#if USED_GRAPHICS_API == METAL_API

#include "mtl_texture.h"

#endif

namespace gb
{
    cubemap_texture_commiter_png::cubemap_texture_commiter_png(const std::string& guid, const resource_shared_ptr& resource) :
    gb::resource_commiter(guid, resource)
    {
        
    }
    
    void cubemap_texture_commiter_png::commit(const resource_transfering_data_shared_ptr& transfering_data)
    {
        m_status = e_commiter_status_in_progress;
        assert(m_resource != nullptr);
        
        const auto texture_transfering_data = std::static_pointer_cast<gb::cubemap_texture_transfering_data>(transfering_data);
        
#if USED_GRAPHICS_API == METAL_API
        
        texture_transfering_data->m_mtl_texture_id = std::make_shared<mtl_texture>(texture_transfering_data->m_size,
                                                                                   texture_transfering_data->m_data);
        
#endif
        
        m_status = e_commiter_status_success;
        
        texture_transfering_data->m_texture_id = 0;
        resource_commiter::on_transfering_data_commited(texture_transfering_data);
    }
}
