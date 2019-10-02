//
//  ui_menus_helper.cpp
//  gbDemo
//
//  Created by serhii.s on 7/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ui_menus_helper.h"
#include "ces_ui_interaction_component.h"
#include "gameplay_ui_fabricator.h"
#include "gameplay_fabricator.h"
#include "button.h"
#include "textfield.h"
#include "image_button.h"
#include "progress_bar.h"
#include "ces_user_database_component.h"
#include "ces_garage_database_component.h"
#include "ces_scene_fabricator_component.h"
#include "ces_car_descriptor_component.h"

namespace game
{
    void ui_menus_helper::create_main_menu_ui(const gb::ces_entity_shared_ptr& root,
                                              const gameplay_ui_fabricator_shared_ptr& gameplay_ui_fabricator,
                                              bool hidden)
    {
        const auto user_database_component = root->get_component<ces_user_database_component>();
        const auto garage_database_component = root->get_component<ces_garage_database_component>();
        
        const auto screen_overlay_bottom = gameplay_ui_fabricator->create_screen_overlay_bottom("");
        root->add_child(screen_overlay_bottom);
        
        const auto career_button = gameplay_ui_fabricator->create_career_button("");
        root->add_child(career_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(career_button), 1.f);
        }
        
        const auto career_label = gameplay_ui_fabricator->create_career_label("");
        root->add_child(career_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(career_label), 1.f);
        }
        
        const auto garage_button = gameplay_ui_fabricator->create_open_garage_button("");
        root->add_child(garage_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(garage_button), 1.f);
        }
        
        const auto garage_label = gameplay_ui_fabricator->create_garage_label("");
        root->add_child(garage_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(garage_label), 1.f);
        }
        
        const auto shop_button = gameplay_ui_fabricator->create_open_shop_button("");
        root->add_child(shop_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(shop_button), 1.f);
        }
        
        const auto shop_label = gameplay_ui_fabricator->create_shop_label("");
        root->add_child(shop_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(shop_label), 1.f);
        }
        
#if defined(__OSX__)
        
        const auto quit_game_button = gameplay_ui_fabricator->create_exit_button("");
        root->add_child(quit_game_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(quit_game_button), 1.f);
        }
        
        const auto controls_button = gameplay_ui_fabricator->create_controls_button("");
        root->add_child(controls_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(controls_button), 1.f);
        }
        
        const auto controls_label = gameplay_ui_fabricator->create_controls_label("");
        root->add_child(controls_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(controls_label), 1.f);
        }
        
#endif
        
#if defined(__FACEBOOK_LOGIN__)
        
        const auto facebook_button = gameplay_ui_fabricator->create_facebook_button("");
        root->add_child(facebook_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(facebook_button), 1.f);
        }
        
#endif
        
        const auto goto_racing1_button = gameplay_ui_fabricator->create_goto_racing1_button("");
        root->add_child(goto_racing1_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(goto_racing1_button), 1.f);
        }
        
        const auto goto_racing2_button = gameplay_ui_fabricator->create_goto_racing2_button("");
        root->add_child(goto_racing2_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(goto_racing2_button), 1.f);
        }
        
        const auto career_dialog = gameplay_ui_fabricator->create_career_dialog("");
        root->add_child(career_dialog);
        ui_animation_helper::hide_to_down(std::static_pointer_cast<gb::ui::control>(career_dialog), 1.f);
        
        const auto shop_dialog = gameplay_ui_fabricator->create_shop_dialog("");
        root->add_child(shop_dialog);
        ui_animation_helper::hide_to_down(std::static_pointer_cast<gb::ui::control>(shop_dialog), 1.f);
        
        const auto daily_tasks_dialog = gameplay_ui_fabricator->create_daily_tasks_dialog("");
        root->add_child(daily_tasks_dialog);
        ui_animation_helper::hide_to_down(std::static_pointer_cast<gb::ui::control>(daily_tasks_dialog), 1.f);
        
#if defined(__IOS__)
        
        const auto tickets_label = gameplay_ui_fabricator->create_tickets_label("");
        root->add_child(tickets_label);
        std::string tickets_text = "TICKETS: ";
        tickets_text.append(std::to_string(user_database_component->get_tickets(1)));
        std::static_pointer_cast<gb::ui::textfield>(tickets_label)->set_text(tickets_text);
        
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(tickets_label), 1.f);
        }
        
#endif
        
        const auto cash_label = gameplay_ui_fabricator->create_cash_label("");
        root->add_child(cash_label);
        ui_controls_helper::update_cash_amount(user_database_component->get_cash(1), 0);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(cash_label), 1.f);
        }
        
        const auto stars_progress_bar = gameplay_ui_fabricator->create_stars_progress_bar("");
        root->add_child(stars_progress_bar);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(stars_progress_bar), 1.f);
        }
        
        const auto current_rank = user_database_component->get_rank(1);
        const auto stars_for_rank = user_database_component->get_stars_for_rank(current_rank + 1);
        const auto current_stars_count_for_rank = user_database_component->get_collected_stars(1, current_rank);
        std::static_pointer_cast<gb::ui::progress_bar>(stars_progress_bar)->set_progress(static_cast<f32>(current_stars_count_for_rank) / static_cast<f32>(stars_for_rank));
        
        const auto stars_progress_label = gameplay_ui_fabricator->create_stars_progress_label("");
        root->add_child(stars_progress_label);
        
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(stars_progress_label), 1.f);
        }
        
        const auto stars_progress_info_label = gameplay_ui_fabricator->create_stars_progress_info_label("");
        root->add_child(stars_progress_info_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(stars_progress_info_label), 1.f);
        }
        
        std::stringstream stars_progress_str_stream;
        stars_progress_str_stream<<current_stars_count_for_rank<<"/"<<stars_for_rank;
        std::static_pointer_cast<gb::ui::textfield>(stars_progress_info_label)->set_text(stars_progress_str_stream.str());
        
        const auto rank_info_label = gameplay_ui_fabricator->create_rank_info_label("");
        root->add_child(rank_info_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(rank_info_label), 1.f);
        }
        
        std::stringstream rank_str_stream;
        rank_str_stream<<"RANK: "<<current_rank;
        std::static_pointer_cast<gb::ui::textfield>(rank_info_label)->set_text(rank_str_stream.str());
        
        const auto stars_progress_button = gameplay_ui_fabricator->create_stars_progress_button("");
        root->add_child(stars_progress_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_up(std::static_pointer_cast<gb::ui::control>(stars_progress_button), 1.f);
        }
        
        const auto leaderboard_button = gameplay_ui_fabricator->create_leaderboard_button("");
        root->add_child(leaderboard_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(leaderboard_button), 1.f);
        }
        
        const auto leaderboard_label = gameplay_ui_fabricator->create_leaderboard_label("");
        root->add_child(leaderboard_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(leaderboard_label), 1.f);
        }
        
        const auto daily_tasks_button = gameplay_ui_fabricator->create_daily_tasks_button("");
        root->add_child(daily_tasks_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(daily_tasks_button), 1.f);
        }
        
        const auto daily_tasks_label = gameplay_ui_fabricator->create_daily_tasks_label("");
        root->add_child(daily_tasks_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(daily_tasks_label), 1.f);
        }
        
        const auto screen_overlay = gameplay_ui_fabricator->create_screen_overlay("");
        root->add_child(screen_overlay);
        
        const auto full_tickets_dialog = gameplay_ui_fabricator->create_full_tickets_dialog("");
        root->add_child(full_tickets_dialog);
        
        const auto not_enough_tickets_dialog = gameplay_ui_fabricator->create_not_enough_tickets_dialog("");
        root->add_child(not_enough_tickets_dialog);
        
        const auto cash_shop_dialog = gameplay_ui_fabricator->create_cash_shop_dialog("");
        root->add_child(cash_shop_dialog);
        
#if defined(__OSX__)
        
        const auto quit_game_dialog = gameplay_ui_fabricator->create_exit_game_dialog("");
        root->add_child(quit_game_dialog);
        
        const auto controls_dialog = gameplay_ui_fabricator->create_controls_dialog("");
        root->add_child(controls_dialog);
        
#endif
        
        if (user_database_component->get_ftue_step(1) == 0)
        {
            ui_controls_helper::disable_all_and_focus_on({ces_ui_interaction_component::e_ui::e_ui_goto_racing1_button,
                ces_ui_interaction_component::e_ui::e_ui_goto_racing2_button});
        }
        
        if (user_database_component->get_ftue_step(1) == 1)
        {
            ui_controls_helper::disable_all_and_focus_on({ces_ui_interaction_component::e_ui::e_ui_open_garage_button,
                ces_ui_interaction_component::e_ui::e_ui_garage_label});
        }
    }
    
    void ui_menus_helper::create_garage_menu_ui(const gb::ces_entity_shared_ptr& root,
                                                const gameplay_ui_fabricator_shared_ptr& gameplay_ui_fabricator,
                                                bool hidden)
    {
        const auto user_database_component = root->get_component<ces_user_database_component>();
        const auto garage_database_component = root->get_component<ces_garage_database_component>();
        
        const auto back_from_garage_button = gameplay_ui_fabricator->create_back_from_garage_button("");
        root->add_child(back_from_garage_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(back_from_garage_button), 1.f);
        }
        
        const auto back_from_garage_label = gameplay_ui_fabricator->create_back_from_garage_label("");
        root->add_child(back_from_garage_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(back_from_garage_label), 1.f);
        }
        
        const auto garage_preview_mode_button = gameplay_ui_fabricator->create_garage_preview_mode_button("");
        root->add_child(garage_preview_mode_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(garage_preview_mode_button), 1.f);
        }
        
        const auto zoom_label = gameplay_ui_fabricator->create_zoom_label("");
        root->add_child(zoom_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(zoom_label), 1.f);
        }
        
        const auto body_paint_label = gameplay_ui_fabricator->create_body_paint_label("");
        root->add_child(body_paint_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(body_paint_label), 1.f);
        }
        
        const auto windshieldd_paint_label = gameplay_ui_fabricator->create_windshield_paint_label("");
        root->add_child(windshieldd_paint_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(windshieldd_paint_label), 1.f);
        }
        
        const auto performance_label = gameplay_ui_fabricator->create_performance_label("");
        root->add_child(performance_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(performance_label), 1.f);
        }
        
        const auto buy_upgrade_button = gameplay_ui_fabricator->create_buy_upgrade_button("");
        root->add_child(buy_upgrade_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(buy_upgrade_button), 1.f);
            buy_upgrade_button->visible = false;
        }
        
        const auto buy_upgrade_label = gameplay_ui_fabricator->create_buy_upgrade_label("");
        root->add_child(buy_upgrade_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(buy_upgrade_label), 1.f);
            buy_upgrade_label->visible = false;
        }
        
        const auto next_car_in_garage_button = gameplay_ui_fabricator->create_next_car_in_garage_button("");
        root->add_child(next_car_in_garage_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(next_car_in_garage_button), 1.f);
        }
        
        const auto prev_car_in_garage_button = gameplay_ui_fabricator->create_prev_car_in_garage_button("");
        root->add_child(prev_car_in_garage_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(prev_car_in_garage_button), 1.f);
        }
        
        const auto car_skin_1_button = gameplay_ui_fabricator->create_car_skin_1_button("");
        root->add_child(car_skin_1_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(car_skin_1_button), 1.f);
        }
        
        const auto car_skin_2_button = gameplay_ui_fabricator->create_car_skin_2_button("");
        root->add_child(car_skin_2_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(car_skin_2_button), 1.f);
        }
        
        const auto car_skin_3_button = gameplay_ui_fabricator->create_car_skin_3_button("");
        root->add_child(car_skin_3_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(car_skin_3_button), 1.f);
        }
        
#if defined(__IOS__)
        
        const auto tickets_label = gameplay_ui_fabricator->create_tickets_label("");
        root->add_child(tickets_label);
        std::string tickets_text = "TICKETS: ";
        tickets_text.append(std::to_string(user_database_component->get_tickets(1)));
        std::static_pointer_cast<gb::ui::textfield>(tickets_label)->set_text(tickets_text);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(tickets_label), 1.f);
        }
        
#endif
        
        const auto cash_label = gameplay_ui_fabricator->create_cash_label("");
        root->add_child(cash_label);
        ui_controls_helper::update_cash_amount(user_database_component->get_cash(1), 0);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(cash_label), 1.f);
        }
        
        const auto select_car_button = gameplay_ui_fabricator->create_select_car_button("");
        root->add_child(select_car_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_down(std::static_pointer_cast<gb::ui::control>(select_car_button), 1.f);
        }
        
        std::shared_ptr<ces_garage_database_component::garage_dto::car_dto> selected_car_data = nullptr;
        i32 current_rank = user_database_component->get_rank(1);
        i32 claimed_rank = user_database_component->get_claimed_rank(1);
        if (current_rank != claimed_rank)
        {
            selected_car_data = garage_database_component->get_car(1, current_rank);
        }
        else
        {
            selected_car_data = garage_database_component->get_selected_car(1);
        }
        
        if (selected_car_data->get_id() == garage_database_component->get_selected_car(1)->get_id())
        {
            std::static_pointer_cast<gb::ui::image_button>(select_car_button)->set_image_color(glm::u8vec4(0, 0, 127, 255));
        }
        
        const auto unlock_car_button = gameplay_ui_fabricator->create_unlock_car_button("");
        root->add_child(unlock_car_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_down(std::static_pointer_cast<gb::ui::control>(unlock_car_button), 1.f);
        }
        
        const auto buy_car_button = gameplay_ui_fabricator->create_buy_car_button("");
        root->add_child(buy_car_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(buy_car_button), 1.f);
        }
        
        const auto buy_car_label = gameplay_ui_fabricator->create_buy_car_label("");
        root->add_child(buy_car_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(buy_car_label), 1.f);
        }
        
        const auto screen_overlay = gameplay_ui_fabricator->create_screen_overlay("");
        root->add_child(screen_overlay);
        
        const auto full_tickets_dialog = gameplay_ui_fabricator->create_full_tickets_dialog("");
        root->add_child(full_tickets_dialog);
        
        const auto unlock_car_dialog = gameplay_ui_fabricator->create_unlock_car_dialog("");
        root->add_child(unlock_car_dialog);
        
        const auto cash_shop_dialog = gameplay_ui_fabricator->create_cash_shop_dialog("");
        root->add_child(cash_shop_dialog);
        
        const auto body_paint_dialog = gameplay_ui_fabricator->create_body_paint_dialog("");
        root->add_child(body_paint_dialog);
        ui_animation_helper::hide_to_down(std::static_pointer_cast<gb::ui::control>(body_paint_dialog), 1.f);
        
        const auto windshield_paint_dialog = gameplay_ui_fabricator->create_windshield_paint_dialog("");
        root->add_child(windshield_paint_dialog);
        ui_animation_helper::hide_to_down(std::static_pointer_cast<gb::ui::control>(windshield_paint_dialog), 1.f);
        
        const auto performance_upgrade_dialog = gameplay_ui_fabricator->create_performance_upgrade_dialog("");
        root->add_child(performance_upgrade_dialog);
        ui_animation_helper::hide_to_down(std::static_pointer_cast<gb::ui::control>(performance_upgrade_dialog), 1.f);
        
        fill_selected_car_in_garage_ui(root, nullptr);
        
        if (user_database_component->get_ftue_step(1) == 1)
        {
            ui_controls_helper::disable_all_and_focus_on({ces_ui_interaction_component::e_ui::e_ui_buy_car_button});
        }
    }
    
    void ui_menus_helper::create_in_game_menu_ui(const gb::ces_entity_shared_ptr& root,
                                                 const gameplay_ui_fabricator_shared_ptr& gameplay_ui_fabricator,
                                                 bool hidden)
    {
        const auto cars_list_dialog = gameplay_ui_fabricator->create_cars_list_dialog("");
        root->add_child(cars_list_dialog);
        
        const auto pause_button = gameplay_ui_fabricator->create_in_game_pause_button("");
        root->add_child(pause_button);
        
        const auto countdown_timer_label = gameplay_ui_fabricator->create_countdown_label("");
        root->add_child(countdown_timer_label);
        
        const auto car_damage_label = gameplay_ui_fabricator->create_car_damage_label("");
        root->add_child(car_damage_label);
        
        const auto car_damage_bar = gameplay_ui_fabricator->create_car_damage_bar("");
        root->add_child(car_damage_bar);
        std::static_pointer_cast<gb::ui::progress_bar>(car_damage_bar)->set_progress(.01f);
        
        const auto slow_motion_boost_label = gameplay_ui_fabricator->create_slow_motion_boost_label("");
        root->add_child(slow_motion_boost_label);
        ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(slow_motion_boost_label), 1.f);
        
        const auto slow_motion_boost_trigger_dialog = gameplay_ui_fabricator->create_slow_motion_boost_trigger_dialog("");
        root->add_child(slow_motion_boost_trigger_dialog);
        ui_animation_helper::hide_to_down(std::static_pointer_cast<gb::ui::control>(slow_motion_boost_trigger_dialog), 1.f);
        
        const auto slow_motion_boost_progress_bar = gameplay_ui_fabricator->create_slow_motion_boost_progress_bar("");
        root->add_child(slow_motion_boost_progress_bar);
        std::static_pointer_cast<gb::ui::progress_bar>(slow_motion_boost_progress_bar)->set_progress(.01f);
        ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(slow_motion_boost_progress_bar), 1.f);
        
        const auto speed_up_boost_label = gameplay_ui_fabricator->create_speed_up_boost_label("");
        root->add_child(speed_up_boost_label);
        ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(speed_up_boost_label), 1.f);
        
        const auto speed_up_boost_trigger_label = gameplay_ui_fabricator->create_speed_up_boost_trigger_label("");
        root->add_child(speed_up_boost_trigger_label);
        ui_animation_helper::hide_to_down(std::static_pointer_cast<gb::ui::control>(speed_up_boost_trigger_label), 1.f);
        
        const auto speed_up_boost_progress_bar = gameplay_ui_fabricator->create_speed_up_boost_progress_bar("");
        root->add_child(speed_up_boost_progress_bar);
        std::static_pointer_cast<gb::ui::progress_bar>(speed_up_boost_progress_bar)->set_progress(.01f);
        ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(speed_up_boost_progress_bar), 1.f);
        
        const auto steer_left_label = gameplay_ui_fabricator->create_tutorial_steer_left_label("");
        root->add_child(steer_left_label);
        steer_left_label->visible = false;
        
        const auto steer_right_label = gameplay_ui_fabricator->create_tutorial_steer_right_label("");
        root->add_child(steer_right_label);
        steer_right_label->visible = false;
        
        const auto steer_label = gameplay_ui_fabricator->create_tutorial_steer_label("");
        root->add_child(steer_label);
        steer_label->visible = false;
        
        const auto screen_overlay = gameplay_ui_fabricator->create_screen_overlay("");
        root->add_child(screen_overlay);
        
        const auto pause_menu_dialog = gameplay_ui_fabricator->create_in_game_pause_menu_dialog("");
        root->add_child(pause_menu_dialog);
        
        const auto restart_dialog = gameplay_ui_fabricator->create_in_game_restart_dialog("");
        root->add_child(restart_dialog);
        
        const auto quit_dialog = gameplay_ui_fabricator->create_in_game_quit_dialog("");
        root->add_child(quit_dialog);
        
        const auto win_dialog = gameplay_ui_fabricator->create_win_dialog("");
        root->add_child(win_dialog);
        
        const auto loose_dialog = gameplay_ui_fabricator->create_loose_dialog("");
        root->add_child(loose_dialog);
        
        const auto not_enough_tickets_dialog = gameplay_ui_fabricator->create_not_enough_tickets_dialog("");
        root->add_child(not_enough_tickets_dialog);
    }
    
    void ui_menus_helper::enable_customization_ui()
    {
        ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_car_skin_1_button)->visible = true;
        ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_car_skin_2_button)->visible = true;
        ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_car_skin_3_button)->visible = true;
        
        ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_body_paint_label)->visible = true;
        ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_windshield_paint_label)->visible = true;
        ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_performance_label)->visible = true;
    }
    
    void ui_menus_helper::disable_customization_ui()
    {
        ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_car_skin_1_button)->visible = false;
        ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_car_skin_2_button)->visible = false;
        ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_car_skin_3_button)->visible = false;
        
        ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_body_paint_label)->visible = false;
        ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_windshield_paint_label)->visible = false;
        ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_performance_label)->visible = false;
    }
    
    void ui_menus_helper::fill_selected_car_in_garage_ui(const gb::ces_entity_shared_ptr& root, const gb::ces_entity_shared_ptr& car)
    {
        const auto garage_database_component = root->get_component<ces_garage_database_component>();
        const auto selected_car_id = garage_database_component->get_previewed_car_id();
        const auto selected_car_data = garage_database_component->get_car(1, selected_car_id);
       
        bool is_car_oppenned = garage_database_component->is_car_oppenned(1, selected_car_id);
        bool is_car_bought = selected_car_data->get_is_bought();
        
        if (is_car_bought)
        {
            enable_customization_ui();
        }
        else
        {
            disable_customization_ui();
        }
        
        if (car)
        {
            const auto scene_fabricator_component = root->get_component<ces_scene_fabricator_component>();
            const gameplay_fabricator_shared_ptr gameplay_fabricator = scene_fabricator_component->gameplay_fabricator;
            
            std::stringstream car_configuration_filename;
            car_configuration_filename<<"car_"<<selected_car_id;
            gameplay_fabricator->reconstruct_car_geometry(car, car_configuration_filename.str());
            gameplay_fabricator->customize_car(car, selected_car_data);
            
            const auto car_descriptor_component = car->get_component<ces_car_descriptor_component>();
            const auto car_upgrade = car_descriptor_component->get_car_upgrade();
            car_upgrade->m_car_main_color = selected_car_data->get_car_main_color();
            car_upgrade->m_car_second_color = selected_car_data->get_car_second_color();
            car_upgrade->m_car_speed_upgrade_value = selected_car_data->get_car_speed_upgrade();
            car_upgrade->m_car_handling_upgrade_value = selected_car_data->get_car_handling_upgrade();
            car_upgrade->m_car_rigidity_upgrade_value = selected_car_data->get_car_rigidity_upgrade();
            car_descriptor_component->get_car_upgrade_cache()->apply(car_upgrade);
        }
        
        const auto select_car_button = ui_controls_helper::get_control_as<gb::ui::image_button>(ces_ui_interaction_component::e_ui::e_ui_select_car_button);
        const auto unlock_car_button = ui_controls_helper::get_control_as<gb::ui::image_button>(ces_ui_interaction_component::e_ui::e_ui_unlock_car_button);
        
        if (select_car_button)
        {
            select_car_button->visible = is_car_oppenned && is_car_bought;
            if (selected_car_id == garage_database_component->get_selected_car(1)->get_id())
            {
                select_car_button->set_image_color(glm::u8vec4(0, 0, 127, 255));
            }
            else
            {
                select_car_button->set_image_color(glm::u8vec4(255, 255, 255, 255));
            }
        }
        
        if (unlock_car_button)
        {
            unlock_car_button->visible = !is_car_oppenned;
        }
        
        const auto upgrade_speed_progress_bar = ui_controls_helper::get_control_as<gb::ui::progress_bar>(ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_progress_bar);
        if (upgrade_speed_progress_bar)
        {
            upgrade_speed_progress_bar->set_progress(selected_car_data->get_car_speed_upgrade());
        }
        
        const auto upgrade_handling_progress_bar = ui_controls_helper::get_control_as<gb::ui::progress_bar>(ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_progress_bar);
        if (upgrade_handling_progress_bar)
        {
            upgrade_handling_progress_bar->set_progress(selected_car_data->get_car_handling_upgrade());
        }
        
        const auto upgrade_rigidity_progress_bar = ui_controls_helper::get_control_as<gb::ui::progress_bar>(ces_ui_interaction_component::e_ui::e_ui_upgrade_rigidity_progress_bar);
        if (upgrade_rigidity_progress_bar)
        {
            upgrade_rigidity_progress_bar->set_progress(selected_car_data->get_car_rigidity_upgrade());
        }
        
        const auto buy_car_button = ui_controls_helper::get_control_as<gb::ui::image_button>(ces_ui_interaction_component::e_ui::e_ui_buy_car_button);
        if (buy_car_button)
        {
            buy_car_button->visible = !is_car_bought && is_car_oppenned;
        }
        
        const auto buy_car_label = ui_controls_helper::get_control_as<gb::ui::textfield>(ces_ui_interaction_component::e_ui::e_ui_buy_car_label);
        if (buy_car_label)
        {
            buy_car_label->visible = !is_car_bought && is_car_oppenned;
            buy_car_label->set_text(std::to_string(selected_car_data->get_price()).append(" $"));
        }
    }
}
