//
//  scene_3d_loading_operation.hpp
//  gbCore
//
//  Created by serhii.s on 12/17/18.
//  Copyright © 2018 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_loading_operation.h"

namespace gb
{
    class scene_3d_loading_operation final : public resource_loading_operation
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        scene_3d_loading_operation(const std::string& filename, const resource_shared_ptr& resource);
        ~scene_3d_loading_operation();
        
        void serialize();
        void commit();
    };
};
