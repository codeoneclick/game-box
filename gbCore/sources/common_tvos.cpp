//
//  common_tvos.cpp
//  gbCore
//
//  Created by serhii serhiiv on 4/5/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//
#if defined(__TVOS__)

#include "common.h"
#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#include <uuid/uuid.h>

static std::set<std::string> g_custom_pathes;

std::string bundlepath(void)
{
    std::string path([[[NSBundle mainBundle] resourcePath] UTF8String]);
    path.append("/");
    return path;
};

std::string documentspath(void)
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

#endif