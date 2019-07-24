//
//  ces_car_camera_follow_component.cpp
//  gbDemo
//
//  Created by serhii.s on 5/6/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_camera_follow_component.h"

namespace game
{
    ces_car_camera_follow_component::ces_car_camera_follow_component()
    {
        min_distance_xz_to_look_at.getter([=]() {
            return m_min_distance_xz_to_look_at;
        });
        
        max_distance_xz_to_look_at.getter([=]() {
            return m_max_distance_xz_to_look_at;
        });
        
        min_distance_y_to_look_at.getter([=]() {
            return m_min_distance_y_to_look_at;
        });
        
        max_distance_y_to_look_at.getter([=]() {
            return m_max_distance_y_to_look_at;
        });
        
        preview_mode.getter([=]() {
            return m_preview_mode;
        });
        
        preview_mode.setter([=](e_preview_mode value) {
            if (value == e_preview_mode::e_2)
            {
                m_min_distance_xz_to_look_at = 5.f;
                m_max_distance_xz_to_look_at = 5.f;
                
                m_min_distance_y_to_look_at = 2.5f;
                m_max_distance_y_to_look_at = 2.5f;
            }
            else if (value == e_preview_mode::e_1)
            {
                m_min_distance_xz_to_look_at = 12.f;
                m_max_distance_xz_to_look_at = 12.f;
                
                m_min_distance_y_to_look_at = 9.f;
                m_max_distance_y_to_look_at = 9.f;
            }
            else if(value == e_preview_mode::e_none)
            {
                m_min_distance_xz_to_look_at = 12.f;
                m_max_distance_xz_to_look_at = 18.f;
                
                m_min_distance_y_to_look_at = 22.f;
                m_max_distance_y_to_look_at = 28.f;
            }
            m_preview_mode = value;
        });
    }
}
