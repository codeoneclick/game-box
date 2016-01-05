//
//  shader_loading_operation.h
//  gbCore
//
//  Created by sergey.sergeev on 8/24/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef shader_loading_operation_h
#define shader_loading_operation_h

#include "resource_loading_operation.h"

namespace gb
{
    class shader_loading_operation final : public resource_loading_operation
    {
    private:
        
    protected:
        
        std::string m_vs_filename;
        std::string m_fs_filename;
        
    public:
        
        shader_loading_operation(const std::string& vs_filename,
                                 const std::string& fs_filename,
                                 const resource_shared_ptr& resource);
        ~shader_loading_operation();
        
        void serialize();
        void commit();
    };
};

#endif
