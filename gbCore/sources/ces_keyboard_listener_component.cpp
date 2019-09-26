//
//  ces_keyboard_listener_component.cpp
//  gbCore
//
//  Created by serhii.s on 9/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_keyboard_listener_component.h"
#include "std_extensions.h"

namespace gb
{
    std::string ces_keyboard_listener_component::add_callback(const ces_keyboard_listener_component::t_callback& callback)
    {
        std::string guid = std::get_guid();
        m_callbacks.insert(std::make_pair(guid, callback));
        return guid;
    }
    
    void ces_keyboard_listener_component::remove_callback( const std::string& guid)
    {
        const auto& callback = m_callbacks.find(guid);
        if(callback != m_callbacks.end())
        {
            m_callbacks.erase(callback);
        }
    }
    
    std::list<ces_keyboard_listener_component::t_callback> ces_keyboard_listener_component::get_callbacks() const
    {
        std::list<t_callback> callbacks;
        std:transform(m_callbacks.begin(), m_callbacks.end(), std::back_inserter(callbacks),
                  [](const std::map<std::string, t_callback>::value_type& value) {
                      return value.second;
                  });
        return callbacks;
    }
}
