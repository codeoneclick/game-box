//
//  ces_touch_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ces_touch_component.h"

namespace gb
{
    ces_touch_component::ces_touch_component()
    {
        m_type = e_ces_component_type_touch;
        for(i32 i = 0; i < e_input_state_max; ++i)
        {
            m_responders[i] = false;
        }
    }
    
    ces_touch_component::~ces_touch_component()
    {
        
    }
    
    void ces_touch_component::enable(e_input_state state, bool value)
    {
        m_responders[state] = value;
    }
    
    bool ces_touch_component::is_enabled(e_input_state state)
    {
        return m_responders[state];
    }
}