//
//  ces_character_parts_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace game
{
    class ces_character_parts_component : public gb::ces_base_component
    {
    public:
        
        struct parts
        {
            static const std::string k_bounds_part;
            static const std::string k_body_part;
            static const std::string k_light_source_part;
        };
        
    private:
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_character_parts_component, gb::ces_base_component::g_guids_container)
        ces_character_parts_component();
        ~ces_character_parts_component();
    };
};

