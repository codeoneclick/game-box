//
//  ces_system_modifiers_component.cpp
//  gbCore
//
//  Created by serhii.s on 5/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_system_modifiers_component.h"

namespace gb
{
    ces_system_modifiers_component::ces_system_modifiers_component()
    {
        
    }
    
    void ces_system_modifiers_component::pause_system(uint8_t id, bool value)
    {
        m_pause_status[id] = value;
    }
    
    bool ces_system_modifiers_component::is_system_paused(uint8_t id) const
    {
        bool result = false;
        const auto it = m_pause_status.find(id);
        if (it != m_pause_status.end())
        {
            result = it->second;
        }
        return result;
    }
    
    void ces_system_modifiers_component::cleanup()
    {
        m_is_cleanup = true;
    }
    
    void ces_system_modifiers_component::cleanup_done()
    {
        m_is_cleanup = false;
    }
    
    bool ces_system_modifiers_component::is_cleanup()
    {
        return m_is_cleanup;
    }
}
