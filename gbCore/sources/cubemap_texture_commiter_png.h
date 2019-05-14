//
//  cubemap_texture_commiter_png.h
//  gbCore
//
//  Created by serhii.s on 5/14/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_commiter.h"

namespace gb
{
    class cubemap_texture_commiter_png final : public resource_commiter
    {
    private:
        
    protected:
        
    public:
        
        cubemap_texture_commiter_png(const std::string& guid, const resource_shared_ptr& resource);
        ~cubemap_texture_commiter_png() = default;
        
        void commit(const resource_transfering_data_shared_ptr& transfering_data);
    };
};
