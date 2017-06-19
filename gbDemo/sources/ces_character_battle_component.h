//
//  ces_character_battle_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 6/19/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_character_battle_component : public gb::ces_base_component
    {
    public:
        
    private:
        
    protected:
        
        gb::ces_entity_weak_ptr m_target;
        
    public:
        
        CTTI_CLASS_GUID(ces_character_battle_component, gb::ces_base_component::g_guids_container)
        ces_character_battle_component();
        ~ces_character_battle_component();
        
        std::property_rw<gb::ces_entity_shared_ptr> target;
    };
};
