//
//  map_commiter_tmx.hpp
//  gbCore
//
//  Created by serhii.s on 2/13/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_commiter.h"

namespace gb
{
    class scene_2d_commiter_tmx final : public resource_commiter
    {
    private:
        
    protected:
        
    public:
        
        scene_2d_commiter_tmx(const std::string& guid, const resource_shared_ptr& resource);
        ~scene_2d_commiter_tmx();
        
        void commit(const resource_transfering_data_shared_ptr& transfering_data);
    };
};

