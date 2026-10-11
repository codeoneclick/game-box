//
//  resource_accessor.hpp
//  gbCore
//
//  Created by serhii serhiiv on 11/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "resource_loading_operation.h"

namespace gb
{
    template<typename T_RESOURCE, typename T_LOADING_OPERATION>
    std::shared_ptr<T_RESOURCE> resource_accessor::get_resource(const std::string& filename, bool sync)
    {
        const std::string& guid = filename;
        std::shared_ptr<T_RESOURCE> resource = nullptr;
        std::shared_ptr<resource_loading_operation> operation;
        {
            std::lock_guard<std::mutex> guard(m_mutex);
            const auto cached = m_resources.find(guid);
            if(cached != m_resources.end()) return std::static_pointer_cast<T_RESOURCE>(cached->second);
            resource = std::make_shared<T_RESOURCE>(guid);
            operation = std::make_shared<T_LOADING_OPERATION>(filename, resource);
            m_resources.insert(std::make_pair(guid, resource));
            if(!sync)
            {
                m_operationsQueue.insert(std::make_pair(guid, operation));
                m_operations_to_serialize.push(operation);
            }
        }
        if(!sync) m_work_available.notify_one();
        else
        {
            // GPU commit remains on the calling game-loop thread for sync loads.
            operation->serialize();
            if(operation->get_status() == e_resource_loading_operation_status_waiting) operation->commit();
            std::lock_guard<std::mutex> guard(m_mutex);
            m_resources_need_to_callback.push(std::make_tuple(resource, operation->get_status() == e_resource_loading_operation_status_success));
        }
        return resource;
    }
};
