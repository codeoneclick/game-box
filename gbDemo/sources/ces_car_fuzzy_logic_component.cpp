//
//  ces_car_fuzzy_logic_component.cpp
//  gbDemo
//
//  Created by serhii.s on 4/19/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_fuzzy_logic_component.h"

namespace game
{
    void ces_car_fuzzy_logic_component::set_parameters(i32 engine_load_script,
                                                       i32 engine_volume_idle_script,
                                                       i32 engine_volume_off_low_script,
                                                       i32 engine_volume_off_mid_script,
                                                       i32 engine_volume_off_high_script,
                                                       i32 engine_volume_on_low_script,
                                                       i32 engine_volume_on_mid_script,
                                                       i32 engine_volume_on_high_script)
    {
        m_engine_load_script = engine_load_script;
        m_engine_load_script_executor = ffll_new_child(m_engine_load_script);
        
        m_engine_volume_idle_script = engine_volume_idle_script;
        m_engine_volume_idle_script_executor = ffll_new_child(m_engine_volume_idle_script);
        
        m_engine_volume_off_low_script = engine_volume_off_low_script;
        m_engine_volume_off_low_script_executor = ffll_new_child(m_engine_volume_off_low_script);
        
        m_engine_volume_off_mid_script = engine_volume_off_mid_script;
        m_engine_volume_off_mid_script_executor = ffll_new_child(m_engine_volume_off_mid_script);
        
        m_engine_volume_off_high_script = engine_volume_off_high_script;
        m_engine_volume_off_high_script_executor = ffll_new_child(m_engine_volume_off_high_script);
        
        m_engine_volume_on_low_script = engine_volume_on_low_script;
        m_engine_volume_on_low_script_executor = ffll_new_child(m_engine_volume_on_low_script);
        
        m_engine_volume_on_mid_script = engine_volume_on_mid_script;
        m_engine_volume_on_mid_script_executor = ffll_new_child(m_engine_volume_on_mid_script);
        
        m_engine_volume_on_high_script = engine_volume_on_high_script;
        m_engine_volume_on_high_script_executor = ffll_new_child(m_engine_volume_on_high_script);
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_load_script() const
    {
        return m_engine_load_script;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_load_script_executor() const
    {
        return m_engine_load_script_executor;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_idle_script() const
    {
        return m_engine_volume_idle_script;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_idle_script_executor() const
    {
        return m_engine_volume_idle_script_executor;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_off_low_script() const
    {
        return m_engine_volume_off_low_script;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_off_low_script_executor() const
    {
        return m_engine_volume_off_low_script_executor;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_off_mid_script() const
    {
        return m_engine_volume_off_mid_script;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_off_mid_script_executor() const
    {
        return m_engine_volume_off_mid_script_executor;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_off_high_script() const
    {
        return m_engine_volume_off_high_script;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_off_high_script_executor() const
    {
        return m_engine_volume_off_high_script_executor;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_on_low_script() const
    {
        return m_engine_volume_on_low_script;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_on_low_script_executor() const
    {
        return m_engine_volume_on_low_script_executor;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_on_mid_script() const
    {
        return m_engine_volume_on_mid_script;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_on_mid_script_executor() const
    {
        return m_engine_volume_on_mid_script_executor;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_on_high_script() const
    {
        return m_engine_volume_on_high_script;
    }
    
    i32 ces_car_fuzzy_logic_component::get_engine_volume_on_high_script_executor() const
    {
        return m_engine_volume_on_high_script_executor;
    }
}
