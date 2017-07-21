//
//  ces_character_animation_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/21/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_character_animation_component.h"
#include "ces_animation_3d_mixer_component.h"
#include "ces_entity.h"

namespace game
{
    ces_character_animation_component::ces_character_animation_component()
    {
        
    }

    void ces_character_animation_component::set_3d_entity_linkage(const gb::ces_entity_weak_ptr& linkage)
    {
        m_3d_entity_linkage = linkage;
    }
    
    void ces_character_animation_component::play_animation(const std::string& animation_name, bool is_looped)
    {
        if(!m_3d_entity_linkage.expired())
        {
            const auto& animation_3d_mixer_component = m_3d_entity_linkage.lock()->get_component<gb::ces_animation_3d_mixer_component>();
            animation_3d_mixer_component->set_animation(animation_name, is_looped);
        }
    }
}
