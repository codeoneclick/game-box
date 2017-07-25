//
//  ces_character_state_automat_component.h
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
    class ces_character_state_automat_component : public gb::ces_base_component
    {
    public:
        
        enum e_mode
        {
            e_mode_unknown = -1,
            e_mode_manual = 0,
            e_mode_ai
        };
        
        enum e_state
        {
            e_state_idle = 0,
            e_state_move,
            e_state_attack,
            e_state_chase,
            e_state_dead
        };
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&, e_state)> on_state_changed_callback_t;
        
    private:
        
        e_mode m_mode;
        e_state m_state;
        ai_actions_processor_shared_ptr m_actions_processor;
        std::vector<std::tuple<gb::ces_entity_weak_ptr, on_state_changed_callback_t>> m_on_state_changed_callbacks;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_character_state_automat_component, gb::ces_base_component::g_guids_container)
        ces_character_state_automat_component();
        ~ces_character_state_automat_component() = default;
        
        e_mode get_mode() const;
        void set_mode(e_mode mode);
        
        e_state get_state() const;
        void set_state(e_state state);
        
        ai_actions_processor_shared_ptr get_actions_processor() const;
        
        void add_on_state_changed_callback(const gb::ces_entity_shared_ptr& owner, const on_state_changed_callback_t& callback);
        void remove_on_state_changed_callback(const gb::ces_entity_shared_ptr& owner);
    };
};
