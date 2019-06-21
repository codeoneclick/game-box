//
//  ces_bound_touch_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/14/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_bound_touch_component.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_bound_touch_3d_component.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "std_extensions.h"

namespace gb
{
    ces_bound_touch_component::ces_bound_touch_component() :
    m_mode(e_mode_unknown)
    {
        for(i32 i = 0; i < e_input_state_max; ++i)
        {
            for (i32 j = 0; j < e_input_source_max; j++)
            {
                m_responders[i][j] = false;
            }
        }
    }
    
    ces_bound_touch_component::~ces_bound_touch_component()
    {
        
    }
    
    void ces_bound_touch_component::enable(gb::e_input_state state, gb::e_input_source source, bool value)
    {
        m_responders[state][source] = value;
    }
    
    bool ces_bound_touch_component::is_respond_to(gb::e_input_state state, gb::e_input_source source) const
    {
        return m_responders[state][source];
    }
    
    std::string ces_bound_touch_component::add_callback(e_input_state input_state, const ces_bound_touch_component::t_callback& callback)
    {
        std::string guid = std::get_guid();
        m_callbacks[input_state].insert(std::make_pair(guid, callback));
        return guid;
    }
    
    void ces_bound_touch_component::remove_callback(e_input_state input_state, const std::string& guid)
    {
        const auto& callback = m_callbacks[input_state].find(guid);
        if(callback != m_callbacks[input_state].end())
        {
            m_callbacks[input_state].erase(callback);
        }
    }
    
    void ces_bound_touch_component::remove_callback(const std::string& guid)
    {
        for(i32 i = 0; i < e_input_state_max; ++i)
        {
            ces_bound_touch_component::remove_callback(static_cast<e_input_state>(i), guid);
        }
    }
    
    std::list<ces_bound_touch_component::t_callback> ces_bound_touch_component::get_callbacks(e_input_state input_state) const
    {
        std::list<t_callback> callbacks;
        std:transform(m_callbacks[input_state].begin(), m_callbacks[input_state].end(), std::back_inserter(callbacks),
                  [](const std::map<std::string, t_callback>::value_type& value) {
                      return value.second;
                  });
        return callbacks;
    }
    
    bool ces_bound_touch_component::is_2d() const
    {
         return m_mode == e_mode_2d;
    }
    
    bool ces_bound_touch_component::is_3d() const
    {
         return m_mode == e_mode_3d;
    }
    
    ces_bound_touch_2d_component_shared_ptr ces_bound_touch_component::as_2d()
    {
        return std::static_pointer_cast<ces_bound_touch_2d_component>(shared_from_this());
    }
    
    ces_bound_touch_3d_component_shared_ptr ces_bound_touch_component::as_3d()
    {
        return std::static_pointer_cast<ces_bound_touch_3d_component>(shared_from_this());
    }
};

#endif
