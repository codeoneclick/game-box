//
//  ces_ui_interaction_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/27/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ui_interaction_component.h"

namespace game
{
    std::array<glm::vec2, static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_max)> ces_ui_interaction_component::k_controls_position;
    
    const i32 ces_ui_interaction_component::k_levels_list_dialog_levels_table = 1;
    const i32 ces_ui_interaction_component::k_cars_list_dialog_table = 1;
    const i32 ces_ui_interaction_component::k_pause_menu_dialog_continue_button = 1;
    const i32 ces_ui_interaction_component::k_pause_menu_dialog_restart_button = 2;
    const i32 ces_ui_interaction_component::k_pause_menu_dialog_exit_button = 3;
    const i32 ces_ui_interaction_component::k_restart_dialog_label = 1;
    const i32 ces_ui_interaction_component::k_restart_dialog_yes_button = 2;
    const i32 ces_ui_interaction_component::k_restart_dialog_no_button = 3;
    const i32 ces_ui_interaction_component::k_quit_dialog_label = 1;
    const i32 ces_ui_interaction_component::k_quit_dialog_yes_button = 2;
    const i32 ces_ui_interaction_component::k_quit_dialog_no_button = 3;
    const i32 ces_ui_interaction_component::k_end_game_dialog_continue_button = 1;
    const i32 ces_ui_interaction_component::k_end_game_dialog_restart_button = 2;
    const i32 ces_ui_interaction_component::k_end_game_dialog_one_more_star_button = 3;
    const i32 ces_ui_interaction_component::k_end_game_dialog_place_label = 4;
    const i32 ces_ui_interaction_component::k_end_game_dialog_drift_time_label = 5;
    const i32 ces_ui_interaction_component::k_end_game_dialog_star1_image = 6;
    const i32 ces_ui_interaction_component::k_end_game_dialog_star2_image = 7;
    const i32 ces_ui_interaction_component::k_end_game_dialog_star3_image = 8;
    const i32 ces_ui_interaction_component::k_win_dialog_star1_achievement_label = 9;
    const i32 ces_ui_interaction_component::k_win_dialog_star2_achievement_label = 10;
    const i32 ces_ui_interaction_component::k_win_dialog_star3_achievement_label = 11;
    const i32 ces_ui_interaction_component::k_loose_dialog_title_label = 1;
    const i32 ces_ui_interaction_component::k_loose_dialog_continue_button = 2;
    const i32 ces_ui_interaction_component::k_loose_dialog_restart_button = 3;
    const i32 ces_ui_interaction_component::k_full_tickets_dialog_title = 1;
    const i32 ces_ui_interaction_component::k_full_tickets_dialog_ok_button = 2;
    const i32 ces_ui_interaction_component::k_not_enough_tickets_dialog_label = 1;
    const i32 ces_ui_interaction_component::k_not_enough_tickets_dialog_ok_button = 2;
    const i32 ces_ui_interaction_component::k_not_enough_tickets_dialog_plus_button = 3;
    const i32 ces_ui_interaction_component::k_shop_dialog_table_view = 1;
    const i32 ces_ui_interaction_component::k_car_unlock_dialog_label = 1;
    const i32 ces_ui_interaction_component::k_car_unlock_dialog_ok_button = 2;
    
    ces_ui_interaction_component::ces_ui_interaction_component() :
    m_ui(e_ui::e_ui_unknown)
    {
        
    }
    
    ces_ui_interaction_component::e_ui ces_ui_interaction_component::get_ui() const
    {
        return m_ui;
    }
    
    void ces_ui_interaction_component::set_ui(e_ui ui)
    {
        m_ui = ui;
    }
}
