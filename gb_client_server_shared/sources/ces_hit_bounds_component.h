//
//  ces_hit_bounds_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace game
{
    class ces_hit_bounds_component : public gb::ces_base_component
    {
    private:
        
    protected:
        
        gb::ces_entity_weak_ptr m_owner;
        
    public:
        
        CTTI_CLASS_GUID(ces_hit_bounds_component, gb::ces_base_component::g_guids_container)
        ces_hit_bounds_component();
        ~ces_hit_bounds_component();
        
        std::property_ro<gb::ces_entity_shared_ptr> owner;
        
        void set_parameters(const gb::ces_entity_shared_ptr& owner);
    };
};
