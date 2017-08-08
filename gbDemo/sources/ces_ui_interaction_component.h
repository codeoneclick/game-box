//
//  ces_ui_interaction_component.h
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
    class ces_ui_interaction_component : public gb::ces_base_component
    {
    public:
        
        enum e_type
        {
            e_type_unknown = -1,
            e_type_attack_button = 0,
            e_type_ability_button,
            e_type_character_avatar_icon,
            e_type_opponent_avatar_icon,
            e_type_quest_dialog,
            e_type_action_console
        };
        
        static const i32 k_quest_dialog_accept_button;
        static const i32 k_quest_dialog_decline_button;
        
    private:
        
        e_type m_type;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_ui_interaction_component, gb::ces_base_component::g_guids_container)
        ces_ui_interaction_component();
        ~ces_ui_interaction_component() = default;
        
        e_type get_type() const;
        void set_type(e_type type);
    };
};

