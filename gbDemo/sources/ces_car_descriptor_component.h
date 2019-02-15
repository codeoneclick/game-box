//
//  ces_car_descriptor_component.hpp
//  gbDemo
//
//  Created by serhii.s on 2/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_car_descriptor_component : public gb::ces_base_component
    {
    private:
    
        glm::vec2 m_position_wc;
        glm::vec2 m_velocity_wc;
        
        f32 m_angular_velocity;
        f32 m_steer_angle;
        
        f32 m_throttle;
        f32 m_brake;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_car_descriptor_component, gb::ces_base_component::g_guids_container)
        ces_car_descriptor_component();
        ~ces_car_descriptor_component() = default;
    };
};

