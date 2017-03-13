//
//  ces_character_controller_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_character_controller_component.h"

namespace game
{
    ces_character_controller_component::ces_character_controller_component() :
    m_mode(e_mode::unknown)
    {
        mode.setter([=](e_mode mode) {
            m_mode = mode;
        });
        
        mode.getter([=]() {
            return m_mode;
        });
    }
    
    ces_character_controller_component::~ces_character_controller_component()
    {
        
    }
}
