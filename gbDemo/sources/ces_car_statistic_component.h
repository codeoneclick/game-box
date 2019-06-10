//
//  ces_character_statistic_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 6/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_car_statistic_component : public gb::ces_base_component
    {
    public:
        
        enum e_mode
        {
            e_mode_unknown = -1,
            e_mode_player = 0,
            e_mode_ai
        };

    private:
        
        e_mode m_mode;
        
    protected:
        
        
    public:
        
        CTTI_CLASS_GUID(ces_car_statistic_component, gb::ces_base_component::g_guids_container)
        ces_car_statistic_component();
        ~ces_car_statistic_component() = default;
        
        std::property_rw<e_mode> mode;
    };
};
