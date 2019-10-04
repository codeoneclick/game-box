//
//  ces_car_impact_component.h
//  gbDemo
//
//  Created by serhii.s on 8/6/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_car_impact_component : public gb::ces_base_component
    {
    private:
        
        const f32 m_slow_motion_effect_duration_in_seconds = 2.0f;
        const f32 m_speed_up_effect_duration_in_seconds = 1.5f;
        const f32 m_shield_effect_duration_in_seconds = 0.f;
        const f32 m_oil_effect_duration_in_secondsd = 0.f;
        
        f32 m_current_slow_motion_effect_duration_in_seconds = 0.f;
        f32 m_current_speed_up_effect_duration_in_seconds = 0.f;
        f32 m_current_shield_effect_duration_in_secondsd = 0.f;
        f32 m_current_oil_effect_duration_in_secondsd = 0.f;
        
        const f32 m_slow_motion_max_impact = 0.33f;
        const f32 m_speed_up_max_impact = 1.66f;
        
        const f32 m_slow_motion_initial_impact = .33f;
        const f32 m_speed_up_initial_impact = 1.66f;
        
        const f32 m_slow_motion_delta_impact = .1f;
        const f32 m_speed_up_delta_impact = 1.f;
        
        f32 m_slow_motion_current_impact = 0.f;
        f32 m_speed_up_current_impact = 0.f;
        
        bool m_is_expect_to_slow_motion_impact = false;
        bool m_is_expect_to_speed_up_impact = false;
        bool m_is_expect_to_shield_impact = false;
        bool m_is_expect_to_oil_impact = false;
        
        bool m_is_slow_motion_boost_ui_shown = false;
        bool m_is_speed_up_boost_ui_shown = false;
        bool m_is_shield_boost_ui_shown = false;
        bool m_is_oil_boost_ui_shown = false;
        
        bool m_is_slow_motion_boost_trigger_ui_shown = false;
        bool m_is_speed_up_boost_trigger_ui_shown = false;
        
    protected:
        
        
    public:
        
        STTI_CLASS_GUID(ces_car_impact_component, gb::ces_base_component::g_guids_container)
        ces_car_impact_component() = default;
        ~ces_car_impact_component() = default;
        
        f32 get_slow_motion_effect_duration_in_seconds() const;
        f32 get_speed_up_effect_duration_in_seconds() const;
        f32 get_shield_effect_duration_in_seconds() const;
        f32 get_oil_effect_duration_in_seconds() const;
        
        f32 get_current_slow_motion_effect_duration_in_seconds() const;
        f32 get_current_speed_up_effect_duration_in_seconds() const;
        f32 get_current_shield_effect_duration_in_seconds() const;
        f32 get_current_oil_effect_duration_in_seconds() const;
        
        void update_current_slow_motion_effect_duration_in_seconds(f32 value);
        void update_current_speed_up_effect_duration_in_seconds(f32 value);
        void update_current_shield_effect_duration_in_seconds(f32 value);
        void update_currect_oil_effect_duration_in_seconds(f32 value);
        
        f32 get_slow_motion_impact_value() const;
        f32 get_speed_up_impact_value() const;
        
        f32 get_slow_motion_impact_progress() const;
        f32 get_speed_up_impact_progress() const;
        f32 get_shield_impact_progress() const;
        f32 get_oil_impact_progress() const;
        
        bool get_is_expect_to_slow_motion_impact() const;
        bool get_is_expect_to_speed_up_impact() const;
        bool get_is_expect_to_shield_impact() const;
        bool get_is_expect_to_oil_impact() const;
        
        void expect_to_slow_motion_impact();
        void expect_to_speed_up_impact();
        void expect_to_shield_impact(f32 value);
        void expect_to_oil_impact(f32 value);
        
        void reset_is_expect_to_slow_motion_impact();
        void reset_is_expect_to_speed_up_impact();
        
        void add_slow_motion_impact();
        void add_speed_up_impact();
        void add_shield_impact();
        void add_oil_impact();
        
        void remove_slow_motion_impact();
        void remove_speed_up_impact();
        void remove_shield_impact();
        void remove_oil_impact();
        
        bool is_slow_motion_impact_exist() const;
        bool is_speed_up_impact_exist() const;
        bool is_shield_impact_exist() const;
        bool is_oil_impact_exist() const;
        
        bool get_is_slow_motion_boost_ui_shown() const;
        bool get_is_speed_up_boost_ui_shown() const;
        bool get_is_shield_boost_ui_shown() const;
        bool get_is_oil_boost_ui_shown() const;
        
        bool get_is_slow_motion_boost_trigger_ui_shown() const;
        bool get_is_speed_up_boost_trigger_ui_shown() const;
        
        void set_is_slow_motion_boost_ui_shown(bool value);
        void set_is_speed_up_boost_ui_shown(bool value);
        
        void set_is_slow_motion_boost_trigger_ui_shown(bool value);
        void set_is_speed_up_boost_trigger_ui_shown(bool value);
    };
};
