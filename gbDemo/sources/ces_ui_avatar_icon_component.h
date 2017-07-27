//
//  ces_ui_avatar_icon_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/27/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_ui_avatar_icon_component : public gb::ces_base_component
    {
    private:
        
        gb::shape_3d_shared_ptr m_avatar_3d_entity;
        gb::ces_entity_shared_ptr m_avatar_2d_entity;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_ui_avatar_icon_component, gb::ces_base_component::g_guids_container)
        ces_ui_avatar_icon_component();
        ~ces_ui_avatar_icon_component() = default;
        
        void set_avatar_3d_entity(const gb::shape_3d_shared_ptr& entity);
        void set_avatar_2d_entity(const gb::ces_entity_shared_ptr& entity);
        
        gb::shape_3d_shared_ptr get_avatar_3d_entity() const;
        gb::ces_entity_shared_ptr get_avatar_2d_entity() const;
    };
};
