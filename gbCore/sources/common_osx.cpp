//
//  common.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//
#if defined(__OSX__)

#include "common.h"
#include <Cocoa/Cocoa.h>
#include <uuid/uuid.h>
#include <sys/time.h>

static std::set<std::string> g_custom_pathes;

std::string bundlepath()
{
    std::string path([[[NSBundle mainBundle] resourcePath] UTF8String]);
    path.append("/");
    return path;
};

std::string executablepath()
{
    std::string path([[[[[[NSBundle mainBundle] resourcePath]
                         stringByDeletingLastPathComponent]
                        stringByDeletingLastPathComponent]
                       stringByDeletingLastPathComponent] UTF8String]);
    path.append("/");
    return path;
};

std::string documentspath()
{
    NSArray *paths = NSSearchPathForDirectoriesInDomains(NSDocumentDirectory, NSUserDomainMask, YES);
    NSString *documentsPath = [paths objectAtIndex:0];
    std::string path([documentsPath UTF8String]);
    path.append("/");
    return path;
}

void add_custom_path(const std::string& path)
{
    g_custom_pathes.insert(path);
};

void remove_custom_path(const std::string& path)
{
    g_custom_pathes.erase(path);
};

std::set<std::string> custom_pathes()
{
    return g_custom_pathes;
};

std::string udid()
{
    uuid_t uuid;
    uuid_generate_random ( uuid );
    char str_udid[37];
    uuid_unparse (uuid, str_udid);
    return str_udid;
};

f64 get_current_time()
{
    struct timeval time_value;
    gettimeofday(&time_value, nullptr);
    return (f64)time_value.tv_sec + (f64)time_value.tv_usec/1000000;
}

#endif
