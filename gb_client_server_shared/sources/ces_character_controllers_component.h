//
//  ces_character_controllers_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_character_controllers_component : public gb::ces_base_component
    {
    public:
        
        enum e_mode
        {
            e_mode_unknown = -1,
            e_mode_manual = 1,
            e_mode_ai
        };
        
    private:
        
    protected:
        
        e_mode m_mode;
        
        information_bubble_controller_weak_ptr m_information_bubble_controller;
        bloodprint_controller_weak_ptr m_bloodprint_controller;
        footprint_controller_weak_ptr m_footprint_controller;
        
    public:
        
        CTTI_CLASS_GUID(ces_character_controllers_component, gb::ces_base_component::g_guids_container)
        ces_character_controllers_component();
        ~ces_character_controllers_component();
        
        std::property_rw<information_bubble_controller_shared_ptr> information_bubble_controller;
        std::property_rw<bloodprint_controller_shared_ptr> bloodprint_controller;
        std::property_rw<footprint_controller_shared_ptr> footprint_controller;
        
        std::property_rw<e_mode> mode;
    };
};
