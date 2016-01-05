//
//  resource_status.h
//  gbCore
//
//  Created by Sergey Sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef resource_status_h
#define resource_status_h

namespace gb
{
    enum e_resource_status
    {
        e_resource_status_unloaded = 1,
        e_resource_status_loaded = 2,
        e_resource_status_commited = 4
    };
};

#endif
