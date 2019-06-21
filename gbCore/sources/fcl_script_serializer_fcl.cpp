//
//  fcl_script_serializer_fcl.cpp
//  gbCore
//
//  Created by serhii.s on 4/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "fcl_script_serializer_fcl.h"
#include "fcl_script.h"
#include "common.h"

namespace gb
{
    fcl_script_serializer_fcl::fcl_script_serializer_fcl(const std::string& filename,
                                                         const resource_shared_ptr& resource) :
    resource_serializer(filename, resource),
    m_filename(filename)
    {
        
    }
    
    void fcl_script_serializer_fcl::serialize(const resource_transfering_data_shared_ptr& transfering_data)
    {
        assert(m_resource != nullptr);
        m_status = e_serializer_status_in_progress;
        
        i32 model = ffll_new_model();
        i32 ret_val = (i32)ffll_load_fcl_file(model, bundlepath().append(m_filename).c_str());
        if (ret_val < 0)
        {
            assert(false);
            m_status = e_serializer_status_failure;
        }
        else
        {
            m_status = e_serializer_status_success;
        }
        const auto fcl_script_transfering_data = std::make_shared<gb::fcl_script_transfering_data>(model);
        m_status = e_serializer_status_success;
        resource_serializer::on_transfering_data_serialized(fcl_script_transfering_data);
    }
}
