//
//  ces_bound_touch_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_bound_touch_component.h"

namespace gb
{
    ces_bound_touch_component::ces_bound_touch_component() :
    m_frame(0.f)
    {
        m_type = e_ces_component_type_bound_touch;
        for(i32 i = 0; i < e_input_state_max; ++i)
        {
            m_responders[i] = false;
            m_callbacks[i] = nullptr;
        }
    }
    
    ces_bound_touch_component::~ces_bound_touch_component()
    {
        
    }
    
    void ces_bound_touch_component::set_frame(const glm::vec4& frame)
    {
        m_frame = frame;
    }
    
    glm::vec4 ces_bound_touch_component::get_frame() const
    {
        return m_frame;
    }
    
    void ces_bound_touch_component::enable(e_input_state state, bool value)
    {
        m_responders[state] = value;
    }
    
    bool ces_bound_touch_component::is_enabled(e_input_state state) const
    {
        return m_responders[state];
    }
    
    void ces_bound_touch_component::set_callback(e_input_state input_state, const ces_bound_touch_component::t_callback& callback)
    {
        m_callbacks[input_state] = callback;
    }
    
    ces_bound_touch_component::t_callback ces_bound_touch_component::get_callback(e_input_state input_state) const
    {
        return m_callbacks[input_state];
    }
};