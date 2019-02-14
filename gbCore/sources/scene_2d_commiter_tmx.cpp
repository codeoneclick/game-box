//
//  map_commiter_tmx.cpp
//  gbCore
//
//  Created by serhii.s on 2/13/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "scene_2d_commiter_tmx.h"

namespace gb
{
    scene_2d_commiter_tmx::scene_2d_commiter_tmx(const std::string& guid, const resource_shared_ptr& resource) :
    resource_commiter(guid, resource)
    {
        
    }
    
    scene_2d_commiter_tmx::~scene_2d_commiter_tmx()
    {
        
    }
    
    void scene_2d_commiter_tmx::commit(const resource_transfering_data_shared_ptr& transfering_data)
    {
        m_status = e_commiter_status_in_progress;
        assert(m_resource != nullptr);
        m_status = e_commiter_status_success;
    }
}
