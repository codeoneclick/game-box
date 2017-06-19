//
//  ces_character_battle_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 6/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_character_battle_component.h"

namespace game
{
    ces_character_battle_component::ces_character_battle_component()
    {
        target.setter([=](const gb::ces_entity_shared_ptr& target) {
            m_target = target;
        });
        
        target.getter([=]() {
            return m_target.lock();
        });
    }
    
    ces_character_battle_component::~ces_character_battle_component()
    {
        
    }
}
