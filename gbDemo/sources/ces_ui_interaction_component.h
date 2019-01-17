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
            e_type_move_joystick = 0,
            e_type_attack_button,
            e_type_ability_button,
            e_type_character_avatar_icon,
            e_type_opponent_avatar_icon,
            e_type_quest_dialog,
            e_type_questlog_button,
            e_type_questlog_dialog,
            e_type_action_console
        };
        
        static const i32 k_quests_dialog_close_button;
        static const i32 k_quests_dialog_quests_table;
        static const i32 k_quests_dialog_title_label;
        static const i32 k_quests_dialog_biography_label;
        static const i32 k_questlog_dialog_quests_table;
        static const i32 k_questlog_dialog_close_button;
        static const i32 k_questlog_dialog_no_quests_label;
        
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

