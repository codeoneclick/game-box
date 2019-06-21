//
//  fcl_script_loading_operation.cpp
//  gbCore
//
//  Created by serhii.s on 4/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "fcl_script_loading_operation.h"
#include "fcl_script.h"
#include "fcl_script_serializer_fcl.h"
#include "fcl_script_commiter_fcl.h"

namespace gb
{
    fcl_script_loading_operation::fcl_script_loading_operation(const std::string& filename, const resource_shared_ptr& resource) :
    gb::resource_loading_operation(filename, resource),
    m_filename(filename)
    {
        m_transfering_data = nullptr;
    }
    
    void fcl_script_loading_operation::serialize()
    {
        assert(m_resource);
        m_status = e_resource_loading_operation_status_in_progress;
        
        if(m_filename.find(".fcl") != std::string::npos)
        {
            m_serializer = std::make_shared<fcl_script_serializer_fcl>(m_filename, m_resource);
        }
        else
        {
            assert(false);
        }
        
        m_serializer->serialize(m_transfering_data);
        m_status = m_serializer->get_status() == e_serializer_status_success ? e_resource_loading_operation_status_waiting : e_resource_loading_operation_status_failure;
    }
    
    void fcl_script_loading_operation::commit()
    {
        assert(m_resource);
        assert(m_resource->is_loaded());
        
        if(m_filename.find(".fcl") != std::string::npos)
        {
            m_commiter = std::make_shared<fcl_script_commiter_fcl>(m_serializer->get_guid(), m_resource);
        }
        else
        {
            assert(false);
        }
        
        m_commiter->commit(m_transfering_data);
        m_status = m_commiter->get_status() == e_commiter_status_success ? e_resource_loading_operation_status_success : e_resource_loading_operation_status_failure;
    }
}
