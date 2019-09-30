//
//  ces_level_item_shield_component.h
//  gbDemo
//
//  Created by serhii.s on 9/30/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_level_item_shield_component : public gb::ces_base_component
    {
    private:
        
        f32 m_shield_duration_in_seconds = 0.f;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_level_item_shield_component, gb::ces_base_component::g_guids_container)
        ces_level_item_shield_component();
        ~ces_level_item_shield_component() = default;
    };
};

