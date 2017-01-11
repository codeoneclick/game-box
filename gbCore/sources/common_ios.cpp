//
//  common_ios.cpp
//  gbCore
//
//  Created by Sergey Sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//
#if defined(__IOS__)

#include "common.h"
#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>

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

#endif