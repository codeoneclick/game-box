//
//  fcl_script_commiter_fcl.h
//  gbCore
//
//  Created by serhii.s on 4/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_commiter.h"

namespace gb
{
    class fcl_script_commiter_fcl final : public resource_commiter
    {
    private:
        
    protected:
        
    public:
        
        fcl_script_commiter_fcl(const std::string& guid, const resource_shared_ptr& resource);
        ~fcl_script_commiter_fcl() = default;
        
        void commit(const resource_transfering_data_shared_ptr& transfering_data);
    };
};


