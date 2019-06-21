//
//  gameplay_constants_helper.cpp
//  gbDemo
//
//  Created by serhii.s on 6/21/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "gameplay_constants_helper.h"

namespace game
{
    f32 gameplay_constants_helper::m_min_speed_multiplier_first_place_complexity_0 = .6f;
    f32 gameplay_constants_helper::m_min_speed_multiplier_first_place_complexity_1 = .8f;
    
    f32 gameplay_constants_helper::m_max_speed_multiplier_first_place_complexity_0 = .8f;
    f32 gameplay_constants_helper::m_max_speed_multiplier_first_place_complexity_1 = 1.f;
    
    f32 gameplay_constants_helper::m_min_speed_multiplier_last_place_complexity_0 = .8f;
    f32 gameplay_constants_helper::m_min_speed_multiplier_last_place_complexity_1 = 1.f;
    
    f32 gameplay_constants_helper::m_max_speed_multiplier_last_place_complexity_0 = 1.f;
    f32 gameplay_constants_helper::m_max_speed_multiplier_last_place_complexity_1 = 1.2f;
}
