//
//  ces_heightmap_configuration_component.h
//  gbCore
//
//  Created by serhii serhiiv on 8/29/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_heightmap_configuration_component : public ces_base_component
    {
    private:
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_heightmap_configuration_component, ces_base_component::g_guids_container)
        ces_heightmap_configuration_component();
        ~ces_heightmap_configuration_component();
    };
};