//
//  animation_commiter_seq.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "animation_commiter_seq.h"

namespace gb
{
    animation_commiter_seq::animation_commiter_seq(const std::string& guid, const resource_shared_ptr& resource) :
    gb::resource_commiter(guid, resource)
    {
        
    }
    
    animation_commiter_seq::~animation_commiter_seq(void)
    {
        
    }
    
    void animation_commiter_seq::commit(void)
    {
        m_status = e_commiter_status_in_progress;
        assert(m_resource != nullptr);
        m_status = e_commiter_status_success;
    }
}