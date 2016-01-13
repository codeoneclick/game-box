//
//  common.h
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef common_h
#define common_h

#include "main_headers.h"

std::string bundlepath();
std::string executablepath();
std::string documentspath();

void add_custom_path(const std::string& path);
void remove_custom_path(const std::string& path);

std::set<std::string> custom_pathes();

#endif
