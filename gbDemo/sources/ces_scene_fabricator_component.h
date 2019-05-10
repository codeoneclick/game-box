//
//  ces_scene_fabricator_component.h
//  gbDemo
//
//  Created by serhii.s on 5/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_scene_fabricator_component : public gb::ces_base_component
    {
    private:
        
        gameplay_fabricator_weak_ptr m_gameplay_fabricator;
        gameplay_ui_fabricator_weak_ptr m_gameplay_ui_fabricator;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_scene_fabricator_component, gb::ces_base_component::g_guids_container)
        ces_scene_fabricator_component();
        ~ces_scene_fabricator_component() = default;
        
        void set_parameters(const gameplay_fabricator_shared_ptr& gameplay_fabricator,
                            const gameplay_ui_fabricator_shared_ptr& gameplay_ui_fabricator);
        
        std::property_ro<gameplay_fabricator_shared_ptr> gameplay_fabricator;
        std::property_ro<gameplay_ui_fabricator_shared_ptr> gameplay_ui_fabricator;
    };
};

