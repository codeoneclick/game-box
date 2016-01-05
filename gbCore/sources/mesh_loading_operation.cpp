//
//  mesh_loading_operation.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/26/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "mesh_loading_operation.h"
#include "mesh_serializer_3dmesh.h"
#include "mesh_commiter_3dmesh.h"
#include "resource.h"

namespace gb
{
    mesh_loading_operation::mesh_loading_operation(const std::string& filename,
                                                   const resource_shared_ptr& resource) :
    gb::resource_loading_operation(filename, resource),
    m_filename(filename)
    {
        
    }
    
    mesh_loading_operation::~mesh_loading_operation()
    {
        
    }
    
    void mesh_loading_operation::serialize()
    {
        assert(m_resource);
        m_status = e_resource_loading_operation_status_in_progress;
        if(m_filename.find(".IE3DMESH"))
        {
            m_serializer = std::make_shared<mesh_serializer_3dmesh>(m_filename,
                                                                    m_resource);
            m_serializer->serialize();
        }
        else
        {
            assert(false);
        }
        m_status = m_serializer->get_status() == e_serializer_status_success && m_status != e_resource_loading_operation_status_failure ? e_resource_loading_operation_status_waiting : e_resource_loading_operation_status_failure;
    }
    
    void mesh_loading_operation::commit()
    {
        assert(m_resource);
        assert(m_resource->is_loaded());
        
        if(m_filename.find(".IE3DMESH"))
        {
            m_commiter = std::make_shared<mesh_commiter_3dmesh>(m_filename,
                                                                m_resource);
            m_commiter->commit();
        }
        else
        {
            assert(false);
        }
        
        m_status = m_commiter->get_status() == e_commiter_status_success && m_status != e_resource_loading_operation_status_waiting ? e_resource_loading_operation_status_success : e_resource_loading_operation_status_failure;
    }
}