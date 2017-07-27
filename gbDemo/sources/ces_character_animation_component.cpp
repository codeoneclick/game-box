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
    const std::string ces_character_animation_component::animations::k_idle_animation = "idle";
    const std::string ces_character_animation_component::animations::k_walk_animation = "move";
    const std::string ces_character_animation_component::animations::k_attack_animation = "attack";
    const std::string ces_character_animation_component::animations::k_search_animation = "search";
    const std::string ces_character_animation_component::animations::k_die_animation = "die";
    
    ces_character_animation_component::ces_character_animation_component()
    {
        
    }

    void ces_character_animation_component::set_3d_entity_linkage(const gb::ces_entity_shared_ptr& linkage)
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
    
    void ces_character_animation_component::add_on_amimation_ended_callback(const gb::ces_entity_shared_ptr& owner, const on_animation_ended_callback_t& callback)
    {
        const auto& animation_3d_mixer_component = m_3d_entity_linkage.lock()->get_component<gb::ces_animation_3d_mixer_component>();
        animation_3d_mixer_component->add_animation_ended_callback(owner, callback);
    }
    
    void ces_character_animation_component::remove_on_animation_ended_callback(const gb::ces_entity_shared_ptr& owner)
    {
        const auto& animation_3d_mixer_component = m_3d_entity_linkage.lock()->get_component<gb::ces_animation_3d_mixer_component>();
        animation_3d_mixer_component->remove_animation_ended_callback(owner);
    }
    
    bool ces_character_animation_component::is_animation_ended_callback_exist(const gb::ces_entity_shared_ptr& owner)
    {
        const auto& animation_3d_mixer_component = m_3d_entity_linkage.lock()->get_component<gb::ces_animation_3d_mixer_component>();
        return animation_3d_mixer_component->is_animation_ended_callback_exist(owner);
    }
}
