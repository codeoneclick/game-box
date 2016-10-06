//
//  ces_base_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/11/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_base_component.h"
#include "game_commands_container.h"

namespace gb
{
    std::set<uintptr_t> ces_base_component::g_guids_container;
    ces_base_component::ces_base_component()
    {
        //std::cout<<__CLASS_NAME__<<std::endl;
    }
};