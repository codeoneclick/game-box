//
//  animation_commiter_ani.hpp
//  gbAnimation
//
//  Created by serhii serhiiv on 11/11/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#ifndef animation_commiter_ani_h
#define animation_commiter_ani_h

#include "resource_commiter.h"

namespace gb
{
    class animation_commiter_ani final : public resource_commiter
    {
    private:
        
    protected:
        
    public:
        
        animation_commiter_ani(const std::string& guid, const resource_shared_ptr& resource);
        ~animation_commiter_ani();
        
        void commit(const resource_transfering_data_shared_ptr& transfering_data);
    };
}

#endif
