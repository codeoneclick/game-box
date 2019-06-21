//
//  ces_track_descriptor_component.cpp
//  gbDemo
//
//  Created by serhii.s on 5/6/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_level_descriptor_component.h"

namespace game
{
    ces_level_descriptor_component::ces_level_descriptor_component()
    {
        is_started.getter([=]() {
            return m_is_started;
        });
        
        is_started.setter([=](f32 value) {
            return m_is_started = value;
        });
        
        is_paused.getter([=]() {
            return m_is_paused;
        });
        
        is_paused.setter([=](f32 value) {
            return m_is_paused = value;
        });
        
        is_win.getter([=]() {
            return m_is_win;
        });
        
        is_win.setter([=](f32 value) {
            m_is_win = value;
        });
        
        is_loose.getter([=]() {
            return m_is_loose;
        });
        
        is_loose.setter([=](f32 value) {
            m_is_loose = value;
        });
        
        start_timestamp.getter([=]() {
            return m_start_timestamp;
        });
        
        start_timestamp.setter([=](f32 value) {
            return m_start_timestamp = value;
        });
        
        countdown_time.getter([=]() {
            return m_countdown_time;
        });
        
        round_time.getter([=]() {
            return m_round_time;
        });
        
        round_time.setter([=](f32 value) {
            m_round_time = value;
        });
        
        current_countdown_time.getter([=]() {
            return m_current_countdown_time;
        });
        
        current_countdown_time.setter([=](f32 value) {
            m_current_countdown_time = value;
        });
        
        current_round_time.getter([=]() {
            return m_current_round_time;
        });
        
        current_round_time.setter([=](f32 value) {
            m_current_round_time = value;
        });
    }
}
