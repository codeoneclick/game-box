//
//  ces_character_controller_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace game
{
    class ces_character_controller_component : public gb::ces_base_component
    {
    public:
        
        enum e_mode
        {
            unknown = -1,
            main = 1,
            ai
        };
        
    private:
        
    protected:
        
        e_mode m_mode;
  
    public:
        
        CTTI_CLASS_GUID(ces_character_controller_component, gb::ces_base_component::g_guids_container)
        ces_character_controller_component();
        ~ces_character_controller_component();
        
        std::property_rw<e_mode> mode;
    };
};
