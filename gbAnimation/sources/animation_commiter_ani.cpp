//
//  animation_commiter_ani.cpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "animation_commiter_ani.h"
#include "animation.h"

namespace gb
{
    animation_commiter_ani::animation_commiter_ani(const std::string& guid, const resource_shared_ptr& resource) :
    gb::resource_commiter(guid, resource)
    {
        
    }
    
    animation_commiter_ani::~animation_commiter_ani()
    {
        
    }
    
    void animation_commiter_ani::commit(const resource_transfering_data_shared_ptr& transfering_data)
    {
        m_status = e_commiter_status_in_progress;
        assert(m_resource != nullptr);

        m_status = e_commiter_status_success;
        resource_commiter::on_transfering_data_commited(transfering_data);
    }
}
