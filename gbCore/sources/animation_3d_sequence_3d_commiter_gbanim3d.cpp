//
//  animation_sequence_3d_commiter_gbanim3d.cpp
//  gbCore
//
//  Created by serhii serhiiv on 5/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "animation_3d_sequence_commiter_gbanim3d.h"

namespace gb
{
    animation_sequence_3d_commiter_gbanim3d::animation_sequence_3d_commiter_gbanim3d(const std::string& guid, const resource_shared_ptr& resource) :
    resource_commiter(guid, resource)
    {
        
    }
    
    animation_sequence_3d_commiter_gbanim3d::~animation_sequence_3d_commiter_gbanim3d()
    {
        
    }
    
    void animation_sequence_3d_commiter_gbanim3d::commit(const resource_transfering_data_shared_ptr& transfering_data)
    {
        m_status = e_commiter_status_in_progress;
        assert(m_resource != nullptr);
        m_status = e_commiter_status_success;
    }
}
