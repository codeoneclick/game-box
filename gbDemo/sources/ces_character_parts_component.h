//
//  ces_character_parts_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace game
{
    class ces_character_parts_component : public gb::ces_base_component
    {
    public:
        
        struct parts
        {
            static const std::string k_body_part;
            static const std::string k_bounds_part;
            static const std::string k_light_source_part;
        };
        
    private:
        
        gb::ces_entity_weak_ptr m_body_part;
        gb::ces_entity_weak_ptr m_bounds_part;
        gb::ces_entity_weak_ptr m_light_source_part;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_character_parts_component, gb::ces_base_component::g_guids_container)
        ces_character_parts_component();
        ~ces_character_parts_component();
        
        void setup(const gb::ces_entity_shared_ptr& body_part,
                   const gb::ces_entity_shared_ptr& bounds_part,
                   const gb::ces_entity_shared_ptr& ligth_source_part);
        
        gb::ces_entity_shared_ptr get_body_part() const;
        gb::ces_entity_shared_ptr get_bounds_part() const;
        gb::ces_entity_shared_ptr get_light_source_part() const;
    };
};

