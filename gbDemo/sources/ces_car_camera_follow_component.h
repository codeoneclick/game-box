//
//  ces_car_camera_follow_component.h
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
    class ces_car_camera_follow_component : public gb::ces_base_component
    {
    public:
        
        enum class e_preview_mode
        {
            e_none = -1,
            e_1,
            e_2
        };
        
    private:
        
        f32 m_min_distance_xz_to_look_at = 12.f;
        f32 m_max_distance_xz_to_look_at = 18.f;
        
        f32 m_min_distance_y_to_look_at = 22.f;
        f32 m_max_distance_y_to_look_at = 28.f;
        
        e_preview_mode m_preview_mode = e_preview_mode::e_none;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_car_camera_follow_component, gb::ces_base_component::g_guids_container)
        ces_car_camera_follow_component();
        ~ces_car_camera_follow_component() = default;
        
        std::property_ro<f32> min_distance_xz_to_look_at;
        std::property_ro<f32> max_distance_xz_to_look_at;
        std::property_ro<f32> min_distance_y_to_look_at;
        std::property_ro<f32> max_distance_y_to_look_at;
        std::property_rw<e_preview_mode> preview_mode;
    };
};

