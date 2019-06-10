//
//  ces_character_statistic_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 6/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_car_statistic_component.h"

namespace game
{
    ces_car_statistic_component::ces_car_statistic_component() :
    m_mode(e_mode::e_mode_unknown)
    {
        mode.setter([=](e_mode mode) {
            m_mode = mode;
        });
        
        mode.getter([=]() {
            return m_mode;
        });
    }
}
