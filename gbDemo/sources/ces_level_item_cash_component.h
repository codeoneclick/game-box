//
//  ces_level_item_cash_component.h
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
    class ces_level_item_cash_component : public gb::ces_base_component
    {
    private:
        
        i32 m_cash_count = 0;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_level_item_cash_component, gb::ces_base_component::g_guids_container)
        ces_level_item_cash_component();
        ~ces_level_item_cash_component() = default;
    };
};

