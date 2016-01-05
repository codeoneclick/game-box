//
//  resource_commiter.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/18/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "resource_commiter.h"
#include "resource.h"

namespace gb
{
    resource_commiter::resource_commiter(const std::string& guid, const std::shared_ptr<resource>& resource) :
    m_guid(guid),
    m_resource(resource),
    m_status(e_commiter_status_undefined)
    {
        
    }
    
    resource_commiter::~resource_commiter()
    {
        
    }
    
    void resource_commiter::on_transfering_data_commited(const resource_transfering_data_shared_ptr& transfering_data)
    {
        m_resource->on_transfering_data_commited(transfering_data);
    }
    
    std::string resource_commiter::get_guid() const
    {
        return m_guid;
    }
    
    e_commiter_status resource_commiter::get_status() const
    {
        return m_status;
    }
}