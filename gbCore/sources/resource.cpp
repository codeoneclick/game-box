//
//  resource.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "resource.h"
#include "resource_status.h"

namespace gb
{
    static i32 resources_count = 0;
    
    resource_transfering_data::resource_transfering_data()
    {
        m_type = e_resource_transfering_data_type_undefined;
    }
    
    e_resource_transfering_data_type resource_transfering_data::get_type() const
    {
        return m_type;
    }
    
    resource::resource(e_resource_type type, const std::string& guid) :
    m_type(type),
    m_guid(guid),
    m_status(e_resource_status_unloaded)
    {
        resources_count++;
        // std::cout<<"resources count: "<<resources_count<<std::endl;
    }
    
    resource::~resource()
    {
        resources_count--;
        // std::cout<<"resources count: "<<resources_count<<std::endl;
    }
    
    const std::string& resource::get_guid() const
    {
        return m_guid;
    }
    
    e_resource_type resource::get_type() const
    {
        return m_type;
    }
    
    bool resource::is_loaded(void) const
    {
        const bool value = 0 != (m_status & e_resource_status_loaded);
        return value;
    };
    
    bool resource::is_commited(void) const
    {
        const bool value = 0 != (m_status & e_resource_status_commited);
        return value;
    };
    
    void resource::add_resource_loading_listener(const resource_loading_interface_shared_ptr& listener)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            listener->on_resource_loaded(shared_from_this(), true);
        }
        else
        {
            m_listeners.insert(listener);
        }
    }
    
    void resource::remove_resource_loading_listener(const resource_loading_interface_shared_ptr& listener)
    {
        m_listeners.erase(listener);
    }
    
    void resource::add_resource_loading_callback(const std::shared_ptr<f_resource_loading_callback>& callback)
    {
        if(resource::is_loaded() && resource::is_commited())
        {
            (*callback.get())(shared_from_this(), true);
        }
        else
        {
            m_callbacks.insert(callback);
        }
    }
    
    void resource::remove_resource_loading_callback(const std::shared_ptr<f_resource_loading_callback>& callback)
    {
        m_callbacks.erase(callback);
    }
    
    void resource::on_resource_loaded(bool success)
    {
        for(const auto& listener : m_listeners)
        {
            listener->on_resource_loaded(shared_from_this(), success);
        }
        
        for(const auto& callback : m_callbacks)
        {
            (*callback.get())(shared_from_this(), success);
        }
        
        m_listeners.clear();
        m_callbacks.clear();
    }
}
