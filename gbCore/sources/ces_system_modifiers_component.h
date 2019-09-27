//
//  ces_system_modifiers_component.h
//  gbCore
//
//  Created by serhii.s on 5/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_system_modifiers_component : public ces_base_component
    {
    private:
        
        std::unordered_map<uint8_t, bool> m_pause_status;
        bool m_is_cleanup = false;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_system_modifiers_component, ces_base_component::g_guids_container)
        ces_system_modifiers_component();
        ~ces_system_modifiers_component() = default;

        void pause_system(uint8_t id, bool value);
        bool is_system_paused(uint8_t id) const;
        
        void cleanup();
        void cleanup_done();
        bool is_cleanup();
    };
};

