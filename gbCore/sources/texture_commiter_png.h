//
//  texture_commiter_png.h
//  gbCore
//
//  Created by sergey.sergeev on 8/18/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_commiter.h"

namespace gb
{
    class texture_commiter_png final : public resource_commiter
    {
    private:
        
    protected:
        
    public:
        
        texture_commiter_png(const std::string& guid, const resource_shared_ptr& resource);
        ~texture_commiter_png();
        
        void commit(const resource_transfering_data_shared_ptr& transfering_data);
    };
};
