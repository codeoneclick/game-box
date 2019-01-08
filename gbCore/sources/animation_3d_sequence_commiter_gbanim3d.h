//
//  animation_sequence_3d_commiter_gbanim3d.h
//  gbCore
//
//  Created by serhii serhiiv on 5/12/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource_commiter.h"

namespace gb
{
    class animation_sequence_3d_commiter_gbanim3d final : public resource_commiter
    {
    private:
        
    protected:
        
    public:
        
        animation_sequence_3d_commiter_gbanim3d(const std::string& guid, const resource_shared_ptr& resource);
        ~animation_sequence_3d_commiter_gbanim3d();
        
        void commit(const resource_transfering_data_shared_ptr& transfering_data);
    };
};
