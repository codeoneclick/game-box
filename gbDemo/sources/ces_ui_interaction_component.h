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
            e_ui_open_garage_button,
            e_ui_back_from_garage_button,
            e_ui_levels_list_dialog,
            e_ui_cars_list_dialog,
            e_ui_scores_label,
            e_ui_countdown_label,
            e_ui_next_car_in_garage_button,
            e_ui_prev_car_in_garage_button,
            e_ui_car_skin_1_button,
            e_ui_car_skin_2_button,
            e_ui_car_skin_3_button,
            e_ui_level_tutorial_steer_left_label,
            e_ui_level_tutorial_steer_right_label,
            e_ui_goto_racing_button,
            e_ui_pause_button,
            e_ui_pause_menu_dialog,
            e_ui_restart_dialog,
            e_ui_quit_dialog,
            e_ui_tickets_label,
            e_ui_end_game_dialog
        };
        
        static const i32 k_levels_list_dialog_levels_table;
        static const i32 k_cars_list_dialog_table;
        static const i32 k_pause_menu_dialog_continue_button;
        static const i32 k_pause_menu_dialog_restart_button;
        static const i32 k_pause_menu_dialog_exit_button;
        static const i32 k_restart_dialog_yes_button;
        static const i32 k_restart_dialog_no_button;
        static const i32 k_quit_dialog_yes_button;
        static const i32 k_quit_dialog_no_button;
        static const i32 k_end_game_dialog_continue_button;
        static const i32 k_end_game_dialog_restart_button;
        static const i32 k_end_game_dialog_one_more_star_button;
        static const i32 k_end_game_dialog_place_label;
        static const i32 k_end_game_dialog_drift_time_label;
        static const i32 k_end_game_dialog_star1_image;
        static const i32 k_end_game_dialog_star2_image;
        static const i32 k_end_game_dialog_star3_image;
        
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

