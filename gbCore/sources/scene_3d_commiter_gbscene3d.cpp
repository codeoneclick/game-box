//
//  scene_3d_commiter_gbscene3d.cpp
//  gbCore
//
//  Created by serhii.s on 12/17/18.
//  Copyright © 2018 sergey.sergeev. All rights reserved.
//

#include "scene_3d_commiter_gbscene3d.h"

namespace gb
{
    scene_3d_commiter_gbscene3d::scene_3d_commiter_gbscene3d(const std::string& guid, const resource_shared_ptr& resource) :
    resource_commiter(guid, resource)
    {
        
    }
    
    scene_3d_commiter_gbscene3d::~scene_3d_commiter_gbscene3d()
    {
        
    }
    
    void scene_3d_commiter_gbscene3d::commit(const resource_transfering_data_shared_ptr& transfering_data)
    {
        m_status = e_commiter_status_in_progress;
        assert(m_resource != nullptr);
        m_status = e_commiter_status_success;
    }
}
