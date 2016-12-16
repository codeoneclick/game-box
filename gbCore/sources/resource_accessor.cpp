//
//  resource_accessor.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "resource_accessor.h"
#include "shader_loading_operation.h"
#include "texture_loading_operation.h"
#include "resource.h"
#include "shader.h"
#include "texture.h"

namespace gb
{
    resource_accessor::resource_accessor()
    {
        m_thread_executed = true;
        m_thread = std::thread(&resource_accessor::on_thread_update, this);
    }
    
    resource_accessor::~resource_accessor()
    {
        m_thread_executed = false;
        m_thread.join();
        m_operationsQueue.clear();
        m_resources.clear();
    }
    
    void resource_accessor::on_thread_update()
    {
        pthread_setname_np("gb.core.resources.accessor");
        while (m_thread_executed)
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            for(const auto& iterator : m_operationsQueue)
            {
                resource_loading_operation_shared_ptr operation = iterator.second;
                if(operation->get_status() == e_resource_loading_operation_status_undefined)
                {
                    operation->serialize();
                }
            }
            if(m_operationsQueue.empty())
            {
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
            }
        }
    }
    
    void resource_accessor::on_update(f32 deltatime)
    {
        auto iterator = m_operationsQueue.begin();
        while(iterator != m_operationsQueue.end())
        {
            resource_loading_operation_shared_ptr operation = iterator->second;
            if(operation->get_status() == e_resource_loading_operation_status_waiting)
            {
                operation->commit();
            }
            else if(operation->get_status() == e_resource_loading_operation_status_failure ||
                    operation->get_status() == e_resource_loading_operation_status_success)
            {
                std::lock_guard<std::mutex> guard(m_mutex);
                m_operationsQueue.erase(iterator++);
                
                const auto& resource = m_resources.find(operation->get_guid());
                assert(resource != m_resources.end());
                if(resource != m_resources.end())
                {
                    resource->second->on_resource_loaded(operation->get_status() == e_resource_loading_operation_status_success);
                }
            }
            else
            {
                ++iterator;
            }
        }
        while (!m_resources_need_to_callback.empty())
        {
            std::get<0>(m_resources_need_to_callback.front())->on_resource_loaded(std::get<1>(m_resources_need_to_callback.front()));
            m_resources_need_to_callback.pop();
        }
    }
    
    void resource_accessor::add_custom_resource(const std::string& guid, const resource_shared_ptr& resource)
    {
        m_resources[guid] = resource;
    }
}
