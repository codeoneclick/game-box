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
        
        enum e_ui
        {
            e_ui_unknown = -1,
            e_ui_open_levels_list_dialog_button,
            e_ui_levels_list_dialog,
            e_ui_scores_label,
            e_ui_countdown_label
        };
        
        static const i32 k_quests_dialog_close_button;
        static const i32 k_quests_dialog_quests_table;
        static const i32 k_quests_dialog_title_label;
        static const i32 k_quests_dialog_biography_label;
        static const i32 k_questlog_dialog_quests_table;
        static const i32 k_questlog_dialog_close_button;
        static const i32 k_questlog_dialog_no_quests_label;
        static const i32 k_levels_list_dialog_levels_table;
        
    private:
        
        e_ui m_ui;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_ui_interaction_component, gb::ces_base_component::g_guids_container)
        ces_ui_interaction_component();
        ~ces_ui_interaction_component() = default;
        
        e_ui get_ui() const;
        void set_ui(e_ui ui);
    };
};

