//
//  ces_car_sounds_set_component.h
//  gbDemo
//
//  Created by serhii.s on 4/19/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_car_sounds_set_component : public gb::ces_base_component
    {
    public:
        
        struct sounds
        {
            static const std::string k_engine_idle;
            static const std::string k_engine_off_low;
            static const std::string k_engine_off_mid;
            static const std::string k_engine_off_high;
            static const std::string k_engine_on_low;
            static const std::string k_engine_on_mid;
            static const std::string k_engine_on_high;
            static const std::string k_drift;
        };
        
    private:
        
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_car_sounds_set_component, gb::ces_base_component::g_guids_container)
        ces_car_sounds_set_component() = default;
        ~ces_car_sounds_set_component() = default;
    };
};

