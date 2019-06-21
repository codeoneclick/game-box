//
//  fcl_script_serializer_fcl.h
//  gbCore
//
//  Created by serhii.s on 4/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_serializer.h"

namespace gb
{
    class fcl_script_serializer_fcl final : public resource_serializer
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        fcl_script_serializer_fcl(const std::string& filename, const resource_shared_ptr& resource);
        ~fcl_script_serializer_fcl() = default;
        
        void serialize(const resource_transfering_data_shared_ptr& transfering_data);
    };
};

