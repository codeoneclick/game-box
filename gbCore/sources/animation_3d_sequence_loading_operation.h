//
//  animation_3d_sequence_loading_operation.h
//  gbCore
//
//  Created by serhii serhiiv on 5/15/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_loading_operation.h"

namespace gb
{
    class animation_3d_sequence_loading_operation final : public resource_loading_operation
    {
    private:
        
    protected:
        
        std::string m_filename;
        
    public:
        
        animation_3d_sequence_loading_operation(const std::string& filename, const resource_shared_ptr& resource);
        ~animation_3d_sequence_loading_operation();
        
        void serialize();
        void commit();
    };
};
