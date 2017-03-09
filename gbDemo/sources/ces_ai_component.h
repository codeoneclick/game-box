//
//  ces_ai_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace game
{
    class ces_ai_component : public gb::ces_base_component
    {
    private:
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_ai_component, gb::ces_base_component::g_guids_container)
        ces_ai_component();
        ~ces_ai_component();
    };
};

