//
//  scene_3d_commiter_gbscene3d.hpp
//  gbCore
//
//  Created by serhii.s on 12/17/18.
//  Copyright © 2018 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_commiter.h"

namespace gb
{
    class scene_3d_commiter_gbscene3d final : public resource_commiter
    {
    private:
        
    protected:
        
    public:
        
        scene_3d_commiter_gbscene3d(const std::string& guid, const resource_shared_ptr& resource);
        ~scene_3d_commiter_gbscene3d();
        
        void commit(const resource_transfering_data_shared_ptr& transfering_data);
    };
};

