//
//  ces_character_pathfinder_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/25/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_character_pathfinder_component.h"
#include "pathfinder.h"

namespace game
{
    ces_character_pathfinder_component::ces_character_pathfinder_component() :
    m_pathfinder(std::make_shared<pathfinder>())
    {
        
    }
    
    ces_character_pathfinder_component::~ces_character_pathfinder_component()
    {
        
    }
    
    pathfinder_shared_ptr ces_character_pathfinder_component::get_pathfinder() const
    {
        return m_pathfinder;
    }
}
