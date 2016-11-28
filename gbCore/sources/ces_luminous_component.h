//
//  ces_luminous_component.h
//  gbCore
//
//  Created by serhii serhiiv on 11/21/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_luminous_component : public ces_base_component
    {
    private:
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_luminous_component, ces_base_component::g_guids_container)
        ces_luminous_component();
        ~ces_luminous_component();
    };
};

