//
//  ces_level_tutorial_component.cpp
//  gbDemo
//
//  Created by serhii.s on 5/24/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_level_tutorial_component.h"


namespace game
{
    ces_level_tutorial_component::ces_level_tutorial_component()
    {
        id.getter([=]() {
            return m_id;
        });
        
        step.getter([=]() {
            return m_step;
        });
        
        step.setter([=](i32 value) {
            m_step = value;
        });
        
        is_active.getter([=]() {
            return m_is_active;
        });
        
        is_active.setter([=](bool value) {
            m_is_active = value;
        });
    }
    
    void ces_level_tutorial_component::set_parameters(e_tutorial_id id)
    {
        m_id = id;
    }
}
