//
//  ces_level_controllers_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace game
{
    class ces_level_controllers_component : public gb::ces_base_component
    {
    private:
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_level_controllers_component, gb::ces_base_component::g_guids_container)
        ces_level_controllers_component();
        ~ces_level_controllers_component();
    };
};

