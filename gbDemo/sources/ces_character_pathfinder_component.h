//
//  ces_character_pathfinder_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/25/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_character_pathfinder_component : public gb::ces_base_component
    {
    private:
        
        pathfinder_shared_ptr m_pathfinder;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_character_pathfinder_component, gb::ces_base_component::g_guids_container)
        ces_character_pathfinder_component();
        ~ces_character_pathfinder_component();
        
        pathfinder_shared_ptr get_pathfinder() const;
    };
};
