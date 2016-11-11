//
//  ces_base_system.cpp
//  gbCore
//
//  Created by Sergey Sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_base_system.h"

namespace gb
{
    std::set<uintptr_t> ces_base_system::g_guids_container;
    ces_base_system::ces_base_system() :
    m_priority(0)
    {
        
    }
    
    i32 ces_base_system::get_priority() const
    {
        return m_priority;
    }
}
