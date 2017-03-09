//
//  ces_bullet_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace game
{
    class ces_bullet_component : public gb::ces_base_component
    {
    private:
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_bullet_component, gb::ces_base_component::g_guids_container)
        ces_bullet_component();
        ~ces_bullet_component();
    };
};

