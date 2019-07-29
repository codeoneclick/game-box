//
//  gameplay_ui_fabricator.h
//  gbDemo
//
//  Created by serhii serhiiv on 8/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "ns_declarations.h"
#include "ces_ui_interaction_component.h"

namespace game
{
    class gameplay_ui_fabricator
    {
    public:
        
        static const glm::vec2   k_image_button_size;
        static const glm::vec2   k_label_size;
        static const glm::vec2   k_progress_bar_size;
        
        static const glm::u8vec4 k_control_image_color;
        static const glm::u8vec4 k_control_text_color;
        static const glm::u8vec4 k_control_background_color;
        static const glm::u8vec4 k_control_foreground_color;
        static const glm::u8vec4 k_control_image_disabled_color;
        static const glm::u8vec4 k_control_text_disabled_color;
        static const glm::u8vec4 k_control_background_disabled_color;
        static const glm::u8vec4 k_control_foreground_disabled_color;
        static const glm::u8vec4 k_control_image_focused_color;
        static const glm::u8vec4 k_control_text_focused_color;
        static const glm::u8vec4 k_control_background_focused_color;
        static const glm::u8vec4 k_control_foreground_focused_color;
        
        static const glm::u8vec4 k_control_transparent_color;
        
        
    private:
        
        static void colorize_ui_control(const gb::ui::textfield_shared_ptr& control);
        static void colorize_ui_control(const gb::ui::image_button_shared_ptr& control);
        static void colorize_ui_control(const gb::ui::button_shared_ptr& control);
        static void add_ui_interaction_component(const gb::ces_entity_shared_ptr& control, ces_ui_interaction_component::e_ui ui);
        
    protected:
        
        gb::scene_fabricator_weak_ptr m_general_fabricator;
        gb::ui::ui_fabricator_weak_ptr m_ui_base_fabricator;
        glm::ivec2 m_screen_size;
        
    public:
        
        gameplay_ui_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator,
                               const gb::ui::ui_fabricator_shared_ptr& ui_base_fabricator,
                               const glm::ivec2& screen_size);
        ~gameplay_ui_fabricator() = default;
        
        glm::ivec2 get_screen_size() const;
        
        gb::game_object_2d_shared_ptr create_career_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_open_garage_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_back_from_garage_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_next_car_in_garage_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_prev_car_in_garage_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_levels_list_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_countdown_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_car_skin_1_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_car_skin_2_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_car_skin_3_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_cars_list_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_tutorial_steer_left_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_tutorial_steer_right_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_tutorial_steer_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_goto_racing1_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_goto_racing2_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_in_game_pause_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_in_game_pause_menu_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_in_game_restart_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_in_game_quit_dialog(const std::string& filename);
        
#if defined(__IOS__)
        
        gb::game_object_2d_shared_ptr create_tickets_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_open_shop_button(const std::string& filename);
        
#endif
        
        gb::game_object_2d_shared_ptr create_win_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_loose_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_select_car_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_unlock_car_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_buy_car_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_buy_car_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_stars_progress_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_stars_progress_bar(const std::string& filename);
        gb::game_object_2d_shared_ptr create_stars_progress_info_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_stars_progress_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_rank_info_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_car_damage_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_car_damage_bar(const std::string& filename);
        gb::game_object_2d_shared_ptr create_full_tickets_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_not_enough_tickets_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_shop_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_cash_shop_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_screen_overlay(const std::string& filename);
        
#if defined(__FACEBOOK_LOGIN__)
        
        gb::game_object_2d_shared_ptr create_facebook_button(const std::string& filename);
        
#endif
        
        gb::game_object_2d_shared_ptr create_unlock_car_dialog(const std::string& filename);
        
        
#if defined(__OSX__)
        
        gb::game_object_2d_shared_ptr create_exit_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_exit_game_dialog(const std::string& filename);
        
#endif
        
        gb::game_object_2d_shared_ptr create_career_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_garage_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_back_from_garage_label(const std::string& filename);
        
#if defined(__IOS__)
        
        gb::game_object_2d_shared_ptr create_shop_label(const std::string& filename);
        
#endif
        
        gb::game_object_2d_shared_ptr create_garage_preview_mode_button(const std::string& filename);
        
        gb::game_object_2d_shared_ptr create_apply_body_color_1_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_apply_body_color_2_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_apply_body_color_3_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_apply_body_color_4_button(const std::string& filename);
        
        gb::game_object_2d_shared_ptr create_apply_windows_color_1_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_apply_windows_color_2_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_apply_windows_color_3_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_apply_windows_color_4_button(const std::string& filename);
        
        gb::game_object_2d_shared_ptr create_cash_label(const std::string& filename);
        
        gb::game_object_2d_shared_ptr create_body_paint_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_windshield_paint_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_performance_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_upgrade_speed_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_upgrade_speed_progress_bar(const std::string& filename);
        gb::game_object_2d_shared_ptr create_upgrade_speed_plus_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_upgrade_handling_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_upgrade_handling_progress_bar(const std::string& filename);
        gb::game_object_2d_shared_ptr create_upgrade_handling_plus_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_upgrade_rigidity_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_upgrade_rigidity_progress_bar(const std::string& filename);
        gb::game_object_2d_shared_ptr create_upgrade_rigidity_plus_button(const std::string& filename);
        
        gb::game_object_2d_shared_ptr create_buy_upgrade_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_buy_upgrade_label(const std::string& filename);
    };
};

