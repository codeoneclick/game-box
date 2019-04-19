//
//  fcl_script_commiter_fcl.cpp
//  gbCore
//
//  Created by serhii.s on 4/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "fcl_script_commiter_fcl.h"

namespace gb
{
    fcl_script_commiter_fcl::fcl_script_commiter_fcl(const std::string& guid, const resource_shared_ptr& resource) :
    resource_commiter(guid, resource)
    {
        
    }
    
    void fcl_script_commiter_fcl::commit(const resource_transfering_data_shared_ptr& transfering_data)
    {
        m_status = e_commiter_status_in_progress;
        assert(m_resource != nullptr);
        m_status = e_commiter_status_success;
    }
}
