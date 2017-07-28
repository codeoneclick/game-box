//
//  ces_character_blinking_effect_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_character_blinking_effect_component.h"

namespace game
{
    const i32 ces_character_blinking_effect_component::k_blinking_count = 15;
    const f32 ces_character_blinking_effect_component::k_blinking_timeinterval = 100.f;
    
    ces_character_blinking_effect_component::ces_character_blinking_effect_component() :
    m_blinking_count(15),
    m_blinking_timeinterval(100.f),
    m_blinking_effect_ended_callback(nullptr)
    {
        
    }
    
    ces_character_blinking_effect_component::~ces_character_blinking_effect_component()
    {
        
    }
    
    i32 ces_character_blinking_effect_component::get_blinking_count() const
    {
        return m_blinking_count;
    }
    
    f32 ces_character_blinking_effect_component::get_blinking_timeinterval() const
    {
        return m_blinking_timeinterval;
    }
    
    void ces_character_blinking_effect_component::set_blinking_count(i32 value)
    {
        m_blinking_count = value;
    }
    
    void ces_character_blinking_effect_component::set_blinking_timeinterval(f32 value)
    {
        m_blinking_timeinterval = value;
    }
    
    void ces_character_blinking_effect_component::set_blinking_effect_ended_callback(const on_blinking_effect_ended_callback_t& callback)
    {
        m_blinking_effect_ended_callback = callback;
    }
    
    ces_character_blinking_effect_component::on_blinking_effect_ended_callback_t ces_character_blinking_effect_component::get_blinking_effect_ended_callback() const
    {
        return m_blinking_effect_ended_callback;
    }
}
