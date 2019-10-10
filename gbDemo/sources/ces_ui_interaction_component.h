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
        
        enum class e_ui
        {
            e_ui_unknown = -1,
            e_ui_open_levels_list_dialog_button,
            e_ui_open_garage_button,
            e_ui_open_shop_button,
            e_ui_quit_game_button,
            e_ui_controls_button,
            e_ui_back_from_garage_button,
            e_ui_career_dialog,
            e_ui_shop_dialog,
            e_ui_cash_shop_dialog,
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
            e_ui_level_tutorial_steer_label,
            e_ui_goto_racing1_button,
            e_ui_goto_racing2_button,
            e_ui_pause_button,
            e_ui_pause_menu_dialog,
            e_ui_restart_dialog,
            e_ui_quit_dialog,
            e_ui_tickets_label,
            e_ui_tickets_plus_button,
            e_ui_win_dialog,
            e_ui_loose_dialog,
            e_ui_select_car_button,
            e_ui_unlock_car_button,
            e_ui_stars_progress_label,
            e_ui_stars_progress_info_label,
            e_ui_stars_progress_bar,
            e_ui_stars_progress_button,
            e_ui_rank_info_label,
            e_ui_car_damage_label,
            e_ui_car_damage_bar,
            e_ui_full_tickets_dialog,
            e_ui_not_enough_tickets_dialog,
            e_ui_screen_overlay,
            e_ui_screen_overlay_bottom,
            e_ui_screen_loading_overlay,
            e_ui_screen_loading_label,
            e_ui_facebook_button,
            e_ui_unlock_car_dialog,
            e_ui_quit_game_dialog,
            e_ui_career_label,
            e_ui_controls_label,
            e_ui_garage_label,
            e_ui_back_from_garage_label,
            e_ui_shop_label,
            e_ui_garage_preview_button,
            e_ui_body_paint_color_picker,
            e_ui_body_paint_brightness_slider,
            e_ui_windshield_paint_color_picker,
            e_ui_windshield_brightness_slider,
            e_ui_cash_label,
            e_ui_cash_plus_button,
            e_ui_body_paint_label,
            e_ui_windshield_paint_label,
            e_ui_performance_label,
            e_ui_upgrade_speed_label,
            e_ui_upgrade_speed_progress_bar,
            e_ui_upgrade_speed_plus_button,
            e_ui_upgrade_speed_reset_button,
            e_ui_upgrade_handling_label,
            e_ui_upgrade_handling_progress_bar,
            e_ui_upgrade_handling_plus_button,
            e_ui_upgrade_handling_reset_button,
            e_ui_upgrade_rigidity_label,
            e_ui_upgrade_rigidity_progress_bar,
            e_ui_upgrade_rigidity_plus_button,
            e_ui_upgrade_durability_reset_button,
            e_ui_buy_upgrade_button,
            e_ui_buy_upgrade_label,
            e_ui_buy_car_button,
            e_ui_buy_car_label,
            e_ui_zoom_label,
            e_ui_leaderboard_button,
            e_ui_leaderboard_label,
            e_ui_body_paint_dialog,
            e_ui_windshield_paint_dialog,
            e_ui_performance_upgrade_dialog,
            e_ui_slow_motion_boost_label,
            e_ui_slow_motion_progress_bar,
            e_ui_slow_motion_boost_trigger_dialog,
            e_ui_speed_up_boost_label,
            e_ui_speed_up_boost_progress_bar,
            e_ui_speed_up_boost_trigger_dialog,
            e_ui_controls_dialog,
            e_ui_daily_tasks_button,
            e_ui_daily_tasks_label,
            e_ui_daily_tasks_dialog,
            e_ui_subscription_button,
            e_ui_subscription_label,
            e_ui_subscription_dialog,
            e_ui_max
        };
        
        static const i32 k_career_dialog_table_view;
        static const i32 k_cars_list_dialog_table;
        static const i32 k_pause_menu_dialog_continue_button;
        static const i32 k_pause_menu_dialog_restart_button;
        static const i32 k_pause_menu_dialog_exit_button;
        static const i32 k_restart_dialog_label;
        static const i32 k_restart_dialog_yes_button;
        static const i32 k_restart_dialog_no_button;
        static const i32 k_quit_dialog_label;
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
        static const i32 k_win_dialog_cash_label;
        static const i32 k_win_dialog_star1_achievement_label;
        static const i32 k_win_dialog_star2_achievement_label;
        static const i32 k_win_dialog_star3_achievement_label;
        static const i32 k_win_dialog_twice_cash_button_button;
        static const i32 k_loose_dialog_title_label;
        static const i32 k_loose_dialog_continue_button;
        static const i32 k_loose_dialog_restart_button;
        static const i32 k_full_tickets_dialog_title;
        static const i32 k_full_tickets_dialog_ok_button;
        static const i32 k_not_enough_tickets_dialog_label;
        static const i32 k_not_enough_tickets_dialog_ok_button;
        static const i32 k_not_enough_tickets_dialog_plus_button;
        static const i32 k_shop_dialog_table_view;
        static const i32 k_car_unlock_dialog_label;
        static const i32 k_car_unlock_dialog_ok_button;
        static const i32 k_cash_shop_dialog_table_view;
        static const i32 k_cash_shop_ok_button;
        static const i32 k_body_paint_label;
        static const i32 k_body_paint_color_picker;
        static const i32 k_body_paint_brightness_slider;
        static const i32 k_windshield_paint_label;
        static const i32 k_windshield_paint_color_picker;
        static const i32 k_windshield_paint_brightness_slider;
        static const i32 k_speed_upgrade_label;
        static const i32 k_handling_upgrade_label;
        static const i32 k_durability_upgrade_label;
        static const i32 k_speed_upgrade_reset_button;
        static const i32 k_handling_upgrade_reset_button;
        static const i32 k_durability_upgrade_reset_button;
        static const i32 k_speed_upgrade_plus_button;
        static const i32 k_handling_upgrade_plus_button;
        static const i32 k_durability_upgrade_plus_button;
        static const i32 k_speed_upgrade_progress_bar;
        static const i32 k_handling_upgrade_progress_bar;
        static const i32 k_durability_upgrade_progress_bar;
        static const i32 k_slow_motion_boost_trigger_dialog_label;
        static const i32 k_slow_motion_boost_trigger_dialog_indicator_1;
        static const i32 k_slow_motion_boost_trigger_dialog_indicator_2;
        static const i32 k_slow_motion_boost_trigger_dialog_indicator_3;
        static const i32 k_speed_up_boost_trigger_dialog_label;
        static const i32 k_speed_up_boost_trigger_dialog_indicator_1;
        static const i32 k_speed_up_boost_trigger_dialog_indicator_2;
        static const i32 k_speed_up_boost_trigger_dialog_indicator_3;
        static const i32 k_controls_dialog_description_label;
        static const i32 k_controls_dialog_ok_button;
        static const i32 k_daily_tasks_dialog_table_view;
        static const i32 k_subscription_dialog_label;
        static const i32 k_subscription_dialog_yes_button;
        static const i32 k_subscription_dialog_no_button;
        
        static std::array<glm::vec2, static_cast<i32>(e_ui::e_ui_max)> k_controls_position;
        
    private:
        
        e_ui m_ui;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_ui_interaction_component, gb::ces_base_component::g_guids_container)
        ces_ui_interaction_component();
        ~ces_ui_interaction_component() = default;
        
        e_ui get_ui() const;
        void set_ui(e_ui ui);
    };
};

