//
//  cubemap_texture_loading_operation.h
//  gbCore
//
//  Created by serhii.s on 5/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_loading_operation.h"

namespace gb
{
    class cubemap_texture_loading_operation final : public resource_loading_operation
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        cubemap_texture_loading_operation(const std::string& filename, const resource_shared_ptr& resource);
        ~cubemap_texture_loading_operation() = default;
        
        void serialize();
        void commit();
    };
};

