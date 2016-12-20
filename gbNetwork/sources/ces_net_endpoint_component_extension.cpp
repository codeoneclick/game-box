//
//  ces_net_endpoint_component_extension.cpp
//  gbNetwork
//
//  Created by serhii serhiiv on 12/19/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_net_endpoint_component_extension.h"

namespace gb
{
    namespace net
    {
        ces_net_endpoint_component_extension::ces_net_endpoint_component_extension() :
        m_endpoint_find_callback(nullptr)
        {
            
        }
        
        ces_net_endpoint_component_extension::~ces_net_endpoint_component_extension()
        {
            
        }
        
        void ces_net_endpoint_component_extension::add_endpoint(const std::string &endpoint)
        {
            std::lock_guard<std::recursive_mutex> guard(m_mutex);
            m_endpoints.insert(endpoint);
        }
        
        void ces_net_endpoint_component_extension::set_endpoint_find_callback(const endpoint_find_callback_t &callback)
        {
            m_endpoint_find_callback = callback;
        }
        
        ces_net_endpoint_component_extension::endpoint_find_callback_t ces_net_endpoint_component_extension::get_endpoint_find_callback() const
        {
            return m_endpoint_find_callback;
        }
        
        std::set<std::string> ces_net_endpoint_component_extension::get_endpoints()
        {
            std::lock_guard<std::recursive_mutex> guard(m_mutex);
            return m_endpoints;
        }
    }
}
