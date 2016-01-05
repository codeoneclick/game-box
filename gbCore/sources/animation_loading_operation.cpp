//
//  animation_loading_operation.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "animation_loading_operation.h"
#include "animation_serializer_seq.h"
#include "animation_commiter_seq.h"
#include "sequence.h"

namespace gb
{
    animation_loading_operation::animation_loading_operation(const std::string& filename, const resource_shared_ptr& resource) :
    gb::resource_loading_operation(filename, resource),
    m_filename(filename)
    {
        
    }
    
    animation_loading_operation::~animation_loading_operation()
    {
        
    }
    
    void animation_loading_operation::serialize()
    {
        assert(m_resource != nullptr);
        
        m_status = e_resource_loading_operation_status_in_progress;
        m_serializer = std::make_shared<animation_serializer_seq>(m_filename,  m_resource);
        m_serializer->serialize();
        m_status = m_serializer->get_status() == e_serializer_status_success ? e_resource_loading_operation_status_waiting : e_resource_loading_operation_status_failure;
    }
    
    void animation_loading_operation::commit()
    {
        assert(m_resource != nullptr);
        assert(m_resource->is_loaded());
        
        m_commiter = std::make_shared<animation_commiter_seq>(m_filename, m_resource);
        m_commiter->commit();
        m_status = m_commiter->get_status() == e_commiter_status_success ? e_resource_loading_operation_status_success : e_resource_loading_operation_status_failure;
    }
}