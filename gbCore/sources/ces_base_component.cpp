//
//  ces_base_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_base_component.h"
#include "game_commands_container.h"
#include "ces_entity.h"

namespace gb
{
    static i32 components_count = 0;
    
    std::set<stti_guid_t> ces_base_component::g_guids_container;
    
    ces_base_component::ces_base_component()
    {
        components_count++;
        // std::cout<<"components count: "<<components_count<<std::endl;
    }
    
    ces_base_component::~ces_base_component()
    {
        components_count--;
        // std::cout<<"components count: "<<components_count<<std::endl;
    }
};
