//
//  texture_loading_operation.h
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef texture_loading_operation_h
#define texture_loading_operation_h

#include "resource_loading_operation.h"

namespace gb
{
    class texture_loading_operation final : public resource_loading_operation
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        texture_loading_operation(const std::string& filename, const resource_shared_ptr& resource);
        ~texture_loading_operation();
        
        void serialize();
        void commit();
    };
};

#endif
