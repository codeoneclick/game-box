//
//  ces_ocean_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/13/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ces_ocean_component.h"

namespace gb
{
    ces_ocean_component::ces_ocean_component() :
    m_wave_generator_interval(0.f),
    m_wave_generator_time(0.f)
    {
        m_type = e_ces_component_type_ocean;
    }
    
    ces_ocean_component::~ces_ocean_component()
    {
        
    }
    
    void ces_ocean_component::set_wave_generator_interval(f32 wave_generator_interval)
    {
        m_wave_generator_interval = wave_generator_interval;
    }
    
    f32 ces_ocean_component::get_wave_generator_time() const
    {
        return m_wave_generator_time;
    }
    
    void ces_ocean_component::update(f32 deltatime)
    {
        m_wave_generator_time += m_wave_generator_interval;
    }
}