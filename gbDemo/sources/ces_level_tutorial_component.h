//
//  ces_level_tutorial_component.h
//  gbDemo
//
//  Created by serhii.s on 5/24/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_level_tutorial_component : public gb::ces_base_component
    {
    public:
        
        enum e_tutorial_id
        {
            e_tutorial_id_unknown = -1,
            e_tutorial_id_steer = 0,
        };
        
    private:
        
        e_tutorial_id m_id;
        i32 m_step = 0;
        bool m_is_active = true;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_level_tutorial_component, gb::ces_base_component::g_guids_container)
        ces_level_tutorial_component();
        ~ces_level_tutorial_component() = default;
        
        void set_parameters(e_tutorial_id id);
        
        std::property_ro<e_tutorial_id> id;
        std::property_rw<i32> step;
        std::property_rw<bool> is_active;
    };
};
