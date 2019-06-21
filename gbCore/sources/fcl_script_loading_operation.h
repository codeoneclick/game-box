//
//  fcl_script_loading_operation.h
//  gbCore
//
//  Created by serhii.s on 4/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_loading_operation.h"

namespace gb
{
    class fcl_script_loading_operation final : public resource_loading_operation
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        fcl_script_loading_operation(const std::string& filename, const resource_shared_ptr& resource);
        ~fcl_script_loading_operation() = default;
        
        void serialize();
        void commit();
    };
};
