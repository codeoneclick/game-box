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
    }
}
