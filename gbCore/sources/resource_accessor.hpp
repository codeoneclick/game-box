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
        std::string guid = filename;
        std::shared_ptr<T_RESOURCE> resource = nullptr;
        if(m_resources.find(guid) != m_resources.end())
        {
            resource = std::static_pointer_cast<T_RESOURCE>(m_resources.find(guid)->second);
        }
        else
        {
            resource = std::make_shared<T_RESOURCE>(guid);
            std::shared_ptr<resource_loading_operation> operation = std::static_pointer_cast<resource_loading_operation>(std::make_shared<T_LOADING_OPERATION>(filename,
                                                                                                                                                               resource));
            m_resources.insert(std::make_pair(guid, resource));
            if(!sync)
            {
                m_operationsQueue.insert(std::make_pair(guid, operation));
            }
            else
            {
                operation->serialize();
                operation->commit();
                
                m_resources_need_to_callback.push(std::make_tuple(resource, operation->get_status() == e_resource_loading_operation_status_success));
            }
        }
        return resource;
    }
};
