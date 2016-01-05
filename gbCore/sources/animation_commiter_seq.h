//
//  animation_commiter_seq.h
//  gbCore
//
//  Created by sergey.sergeev on 8/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef animation_commiter_seq_h
#define animation_commiter_seq_h

#include "resource_commiter.h"

namespace gb
{
    class animation_commiter_seq final : public resource_commiter
    {
    private:
        
    protected:
        
    public:
        
        animation_commiter_seq(const std::string& guid, const resource_shared_ptr& resource);
        ~animation_commiter_seq();
        
        void commit();
    };
};

#endif
