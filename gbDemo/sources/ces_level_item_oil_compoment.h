//
//  ces_level_item_oil_compoment.h
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
    class ces_level_item_oil_compoment : public gb::ces_base_component
    {
    private:
        
        f32 m_oil_duration_in_seconds = 0.f;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_level_item_oil_compoment, gb::ces_base_component::g_guids_container)
        ces_level_item_oil_compoment();
        ~ces_level_item_oil_compoment() = default;
    };
};
