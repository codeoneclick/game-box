//
//  resource_loading_operation.h
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef resource_loading_operation_h
#define resource_loading_operation_h

#include "main_headers.h"
#include "declarations.h"

namespace gb
{
    enum e_resource_loading_operation_status
    {
        e_resource_loading_operation_status_undefined = 0,
        e_resource_loading_operation_status_in_progress,
        e_resource_loading_operation_status_waiting,
        e_resource_loading_operation_status_failure,
        e_resource_loading_operation_status_success
    };
    
    class resource_loading_operation
    {
    private:
        
    protected:
        
        resource_serializer_shared_ptr m_serializer;
        resource_commiter_shared_ptr m_commiter;
        resource_shared_ptr m_resource;
        
        std::string m_guid;
        e_resource_loading_operation_status m_status;
        
    public:
        
        resource_loading_operation(const std::string& guid,
                                   const resource_shared_ptr& resource);
        virtual ~resource_loading_operation();
        
        virtual void serialize() = 0;
        virtual void commit() = 0;
        
        std::string get_guid() const;
        e_resource_loading_operation_status get_status() const;
    };
};

#endif
