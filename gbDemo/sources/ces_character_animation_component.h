//
//  ces_character_animation_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/21/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_character_animation_component : public gb::ces_base_component
    {
    public:
    
        struct animations
        {
            static const std::string k_idle_animation;
            static const std::string k_walk_animation;
            static const std::string k_attack_animation;
            static const std::string k_search_animation;
            static const std::string k_die_animation;
        };
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&, const std::string&, bool)> on_animation_ended_callback_t;
        
    private:
        
        gb::ces_entity_weak_ptr m_3d_entity_linkage;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_character_animation_component, gb::ces_base_component::g_guids_container)
        ces_character_animation_component();
        ~ces_character_animation_component() = default;
        
        void set_3d_entity_linkage(const gb::ces_entity_shared_ptr& linkage);
        void play_animation(const std::string& animation_name, bool is_looped = false);
        
        bool is_animation_ended_callback_exist(const gb::ces_entity_shared_ptr& owner);
        void add_on_amimation_ended_callback(const gb::ces_entity_shared_ptr& owner, const on_animation_ended_callback_t& callback);
        void remove_on_animation_ended_callback(const gb::ces_entity_shared_ptr& owner);
    };
};

