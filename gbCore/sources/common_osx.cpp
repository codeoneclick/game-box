//
//  common.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "common.h"
#include <Cocoa/Cocoa.h>

std::string bundlepath(void)
{
    std::string path([[[NSBundle mainBundle] resourcePath] UTF8String]);
    path.append("/");
    return path;
};

std::string executablepath(void)
{
    std::string path([[[[[[NSBundle mainBundle] resourcePath]
                         stringByDeletingLastPathComponent]
                        stringByDeletingLastPathComponent]
                       stringByDeletingLastPathComponent] UTF8String]);
    path.append("/");
    return path;
};
