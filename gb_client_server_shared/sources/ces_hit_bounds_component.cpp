//
//  ces_hit_bounds_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_hit_bounds_component.h"

namespace game
{
    ces_hit_bounds_component::ces_hit_bounds_component()
    {
        owner.getter([=]() {
            return m_owner.lock();
        });
    }
    
    ces_hit_bounds_component::~ces_hit_bounds_component()
    {
        
    }
    
    void ces_hit_bounds_component::set_parameters(const gb::ces_entity_shared_ptr& owner)
    {
        m_owner = owner;
    }
}
