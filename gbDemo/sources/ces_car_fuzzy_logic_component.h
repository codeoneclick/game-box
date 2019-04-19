//
//  ces_car_fuzzy_logic_component.h
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
    class ces_car_fuzzy_logic_component : public gb::ces_base_component
    {
    private:
        
        i32 m_engine_load_script = -1;
        i32 m_engine_load_script_executor = -1;
        
        i32 m_engine_volume_idle_script = -1;
        i32 m_engine_volume_idle_script_executor = -1;
        
        i32 m_engine_volume_off_low_script = -1;
        i32 m_engine_volume_off_low_script_executor = -1;
        
        i32 m_engine_volume_off_mid_script = -1;
        i32 m_engine_volume_off_mid_script_executor = -1;
        
        i32 m_engine_volume_off_high_script = -1;
        i32 m_engine_volume_off_high_script_executor = -1;
        
        i32 m_engine_volume_on_low_script = -1;
        i32 m_engine_volume_on_low_script_executor = -1;
        
        i32 m_engine_volume_on_mid_script = -1;
        i32 m_engine_volume_on_mid_script_executor = -1;
        
        i32 m_engine_volume_on_high_script = -1;
        i32 m_engine_volume_on_high_script_executor = -1;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_car_fuzzy_logic_component, gb::ces_base_component::g_guids_container)
        ces_car_fuzzy_logic_component() = default;
        ~ces_car_fuzzy_logic_component() = default;
        
        void set_parameters(i32 engine_load_script,
                            i32 engine_volume_idle_script,
                            i32 engine_volume_off_low_script,
                            i32 engine_volume_off_mid_script,
                            i32 engine_volume_off_high_script,
                            i32 engine_volume_on_low_script,
                            i32 engine_volume_on_mid_script,
                            i32 engine_volume_on_high_script);
        
        i32 get_engine_load_script() const;
        i32 get_engine_load_script_executor() const;
        
        i32 get_engine_volume_idle_script() const;
        i32 get_engine_volume_idle_script_executor() const;
        
        i32 get_engine_volume_off_low_script() const;
        i32 get_engine_volume_off_low_script_executor() const;
        
        i32 get_engine_volume_off_mid_script() const;
        i32 get_engine_volume_off_mid_script_executor() const;
        
        i32 get_engine_volume_off_high_script() const;
        i32 get_engine_volume_off_high_script_executor() const;
        
        i32 get_engine_volume_on_low_script() const;
        i32 get_engine_volume_on_low_script_executor() const;
        
        i32 get_engine_volume_on_mid_script() const;
        i32 get_engine_volume_on_mid_script_executor() const;
        
        i32 get_engine_volume_on_high_script() const;
        i32 get_engine_volume_on_high_script_executor() const;
    };
};

