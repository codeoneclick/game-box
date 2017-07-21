//
//  ces_character_impact_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/21/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace game
{
    class ces_character_impact_component : public gb::ces_base_component
    {
    private:
        
    protected:
        
        gb::ces_entity_weak_ptr m_executor;
        
    public:
        
        CTTI_CLASS_GUID(ces_character_impact_component, gb::ces_base_component::g_guids_container)
        ces_character_impact_component();
        ~ces_character_impact_component();
        
        std::property_ro<gb::ces_entity_shared_ptr> executor;
        void set_executor(const gb::ces_entity_shared_ptr& executor);
    };
};
