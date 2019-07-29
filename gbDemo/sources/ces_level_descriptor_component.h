//
//  ces_track_descriptor_component.h
//  gbDemo
//
//  Created by serhii.s on 5/6/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_level_descriptor_component : public gb::ces_base_component
    {
    private:
        
        bool m_is_started = false;
        bool m_is_paused = false;
        bool m_is_win = false;
        bool m_is_loose = false;
        f32 m_start_timestamp = 0.f;
        f32 m_complexity = .5f;
        
        f32 m_countdown_time = 3.f;
        f32 m_current_countdown_time = 0.f;
        f32 m_round_time = 93.f;
        f32 m_round_time_delta = 0.f;
        f32 m_current_round_time = 0.f;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_level_descriptor_component, gb::ces_base_component::g_guids_container)
        ces_level_descriptor_component();
        ~ces_level_descriptor_component() = default;
        
        std::property_rw<bool> is_started;
        std::property_rw<bool> is_paused;
        std::property_rw<bool> is_win;
        std::property_rw<bool> is_loose;
        std::property_rw<f32> start_timestamp;
        
        std::property_ro<f32> countdown_time;
        std::property_rw<f32> current_countdown_time;
        std::property_rw<f32> round_time;
        std::property_rw<f32> round_time_delta;
        std::property_rw<f32> complexity;
        std::property_rw<f32> current_round_time;
    };
};
