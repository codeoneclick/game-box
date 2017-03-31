//
//  ces_bullet_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_bullet_component.h"

namespace game
{
    ces_bullet_component::ces_bullet_component()
    {
        owner.getter([=]() {
            return m_owner.lock();
        });
    }
    
    ces_bullet_component::~ces_bullet_component()
    {
        
    }
    
    void ces_bullet_component::set_parameters(const gb::ces_entity_shared_ptr& owner)
    {
        m_owner = owner;
    }
}
