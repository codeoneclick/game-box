//
//  resource_loading_operation.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "resource_loading_operation.h"
#include "resource.h"

namespace gb
{
    resource_loading_operation::resource_loading_operation(const std::string& guid,
                                                           const resource_shared_ptr& resource) :
    m_guid(guid),
    m_resource(resource),
    m_status(e_resource_loading_operation_status_undefined),
    m_serializer(nullptr),
    m_commiter(nullptr)
    {
        
    }
    
    resource_loading_operation::~resource_loading_operation()
    {
        
    }
    
    std::string resource_loading_operation::get_guid() const
    {
        return m_guid;
    }
    
    e_resource_loading_operation_status resource_loading_operation::get_status(void) const
    {
        return m_status;
    }
}