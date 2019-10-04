//
//  ces_car_impact_component.cpp
//  gbDemo
//
//  Created by serhii.s on 8/6/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_impact_component.h"

namespace game
{
    f32 ces_car_impact_component::get_slow_motion_effect_duration_in_seconds() const
    {
        return m_slow_motion_effect_duration_in_seconds;
    }
    
    f32 ces_car_impact_component::get_speed_up_effect_duration_in_seconds() const
    {
        return m_speed_up_effect_duration_in_seconds;
    }
    
    f32 ces_car_impact_component::get_current_slow_motion_effect_duration_in_seconds() const
    {
        return m_current_slow_motion_effect_duration_in_seconds;
    }
    
    f32 ces_car_impact_component::get_current_speed_up_effect_duration_in_seconds() const
    {
        return m_current_speed_up_effect_duration_in_seconds;
    }
    
    void ces_car_impact_component::update_current_slow_motion_effect_duration_in_seconds(f32 value)
    {
        m_current_slow_motion_effect_duration_in_seconds += value;
    }
    
    void ces_car_impact_component::update_current_speed_up_effect_duration_in_seconds(f32 value)
    {
        m_current_speed_up_effect_duration_in_seconds += value;
    }
    
    f32 ces_car_impact_component::get_slow_motion_impact_value() const
    {
        return m_slow_motion_current_impact;
    }
    
    f32 ces_car_impact_component::get_speed_up_impact_value() const
    {
        return m_speed_up_current_impact;
    }
    
    f32 ces_car_impact_component::get_slow_motion_impact_progress() const
    {
        return std::max(0.f, m_current_slow_motion_effect_duration_in_seconds) / m_slow_motion_effect_duration_in_seconds;
    }
    
    f32 ces_car_impact_component::get_speed_up_impact_progress() const
    {
        return std::max(0.f, m_current_speed_up_effect_duration_in_seconds) / m_speed_up_effect_duration_in_seconds;
    }
    
    bool ces_car_impact_component::get_is_expect_to_slow_motion_impact() const
    {
        return m_is_expect_to_slow_motion_impact;
    }
    
    bool ces_car_impact_component::get_is_expect_to_speed_up_impact() const
    {
        return m_is_expect_to_speed_up_impact;
    }
    
    void ces_car_impact_component::expect_to_slow_motion_impact()
    {
        m_is_expect_to_slow_motion_impact = true;
    }
    
    void ces_car_impact_component::expect_to_speed_up_impact()
    {
        m_is_expect_to_speed_up_impact = true;
    }
    
    void ces_car_impact_component::reset_is_expect_to_slow_motion_impact()
    {
        m_is_expect_to_slow_motion_impact = false;
        //m_slow_motion_current_impact = 0.f;
    }
    
    void ces_car_impact_component::reset_is_expect_to_speed_up_impact()
    {
        m_is_expect_to_speed_up_impact = false;
        //m_speed_up_current_impact = 0.f;
    }
    
    void ces_car_impact_component::add_slow_motion_impact()
    {
        m_current_slow_motion_effect_duration_in_seconds = m_slow_motion_effect_duration_in_seconds;
        if (m_slow_motion_current_impact < m_slow_motion_initial_impact)
        {
            m_slow_motion_current_impact = m_slow_motion_initial_impact;
        }
        /*else
        {
            m_slow_motion_current_impact += m_slow_motion_delta_impact;
        }*/
    }
    
    void ces_car_impact_component::add_speed_up_impact()
    {
        m_current_speed_up_effect_duration_in_seconds = m_speed_up_effect_duration_in_seconds;
        if (m_speed_up_current_impact < m_speed_up_initial_impact)
        {
            m_speed_up_current_impact = m_speed_up_initial_impact;
        }
        /*else
        {
            m_speed_up_current_impact += m_speed_up_delta_impact;
        }*/
    }
    
    void ces_car_impact_component::remove_slow_motion_impact()
    {
        m_current_slow_motion_effect_duration_in_seconds = 0.f;
    }
    
    void ces_car_impact_component::remove_speed_up_impact()
    {
        m_current_speed_up_effect_duration_in_seconds = 0.f;
    }
    
    bool ces_car_impact_component::is_slow_motion_impact_exist() const
    {
        return m_current_slow_motion_effect_duration_in_seconds > 0.f;
    }
    
    bool ces_car_impact_component::is_speed_up_impact_exist() const
    {
        return m_current_speed_up_effect_duration_in_seconds > 0.f;
    }
    
    bool ces_car_impact_component::get_is_slow_motion_boost_ui_shown() const
    {
        return m_is_slow_motion_boost_ui_shown;
    }
    
    bool ces_car_impact_component::get_is_speed_up_boost_ui_shown() const
    {
        return m_is_speed_up_boost_ui_shown;
    }
    
    bool ces_car_impact_component::get_is_slow_motion_boost_trigger_ui_shown() const
    {
        return m_is_slow_motion_boost_trigger_ui_shown;
    }
    
    bool ces_car_impact_component::get_is_speed_up_boost_trigger_ui_shown() const
    {
        return m_is_speed_up_boost_trigger_ui_shown;
    }
    
    void ces_car_impact_component::set_is_slow_motion_boost_ui_shown(bool value)
    {
        m_is_slow_motion_boost_ui_shown = value;
    }
    
    void ces_car_impact_component::set_is_speed_up_boost_ui_shown(bool value)
    {
        m_is_speed_up_boost_ui_shown = value;
    }
    
    void ces_car_impact_component::set_is_slow_motion_boost_trigger_ui_shown(bool value)
    {
        m_is_slow_motion_boost_trigger_ui_shown = value;
    }
    
    void ces_car_impact_component::set_is_speed_up_boost_trigger_ui_shown(bool value)
    {
        m_is_speed_up_boost_trigger_ui_shown = value;
    }
}
