//
//  ces_ui_avatar_icon_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/27/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ui_avatar_icon_component.h"

namespace game
{
    ces_ui_avatar_icon_component::ces_ui_avatar_icon_component() :
    m_avatar_3d_entity(nullptr),
    m_avatar_2d_entity(nullptr)
    {
        
    }
    
    void ces_ui_avatar_icon_component::set_avatar_3d_entity(const gb::shape_3d_shared_ptr& entity)
    {
        m_avatar_3d_entity = entity;
    }
    
    void ces_ui_avatar_icon_component::set_avatar_2d_entity(const gb::ces_entity_shared_ptr& entity)
    {
        m_avatar_2d_entity = entity;
    }
    
    gb::shape_3d_shared_ptr ces_ui_avatar_icon_component::get_avatar_3d_entity() const
    {
        return m_avatar_3d_entity;
    }
    
    gb::ces_entity_shared_ptr ces_ui_avatar_icon_component::get_avatar_2d_entity() const
    {
        return m_avatar_2d_entity;
    }
}
