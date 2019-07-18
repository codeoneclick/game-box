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
#include "button.h"
#include "textfield.h"
#include "image_button.h"
#include "progress_bar.h"
#include "ces_user_database_component.h"
#include "ces_garage_database_component.h"

namespace game
{
    void ui_menus_helper::create_main_menu_ui(const gb::ces_entity_shared_ptr& root,
                                              const gameplay_ui_fabricator_shared_ptr& gameplay_ui_fabricator,
                                              bool hidden)
    {
        const auto user_database_component = root->get_component<ces_user_database_component>();
        const auto garage_database_component = root->get_component<ces_garage_database_component>();
        
        const auto career_button = gameplay_ui_fabricator->create_open_levels_list_dialog_button("");
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
        
        
#if defined(__IOS__)
        
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
        
#else
        
        const auto quit_game_button = gameplay_ui_fabricator->create_exit_button("");
        root->add_child(quit_game_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_left(std::static_pointer_cast<gb::ui::control>(quit_game_button), 1.f);
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
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(goto_racing1_button), gameplay_ui_fabricator->get_screen_size(), 1.f);
        }
        
        const auto goto_racing2_button = gameplay_ui_fabricator->create_goto_racing2_button("");
        root->add_child(goto_racing2_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(goto_racing2_button), gameplay_ui_fabricator->get_screen_size(), 1.f);
        }
        
        const auto career_list_dialog = gameplay_ui_fabricator->create_levels_list_dialog("");
        root->add_child(career_list_dialog);
        
        const auto shop_dialog = gameplay_ui_fabricator->create_shop_dialog("");
        root->add_child(shop_dialog);
        
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
        
        const auto stars_progress_bar = gameplay_ui_fabricator->create_stars_progress_bar("");
        root->add_child(stars_progress_bar);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(stars_progress_bar), gameplay_ui_fabricator->get_screen_size(), 1.f);
        }
        
        const auto current_rank = user_database_component->get_rank(1);
        const auto stars_for_rank = user_database_component->get_stars_for_rank(current_rank + 1);
        const auto current_stars_count_for_rank = user_database_component->get_collected_stars(1, current_rank);
        std::static_pointer_cast<gb::ui::progress_bar>(stars_progress_bar)->set_progress(static_cast<f32>(current_stars_count_for_rank) / static_cast<f32>(stars_for_rank));
        
        const auto stars_progress_label = gameplay_ui_fabricator->create_stars_progress_label("");
        root->add_child(stars_progress_label);
        
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(stars_progress_label), gameplay_ui_fabricator->get_screen_size(), 1.f);
        }
        
        const auto stars_progress_info_label = gameplay_ui_fabricator->create_stars_progress_info_label("");
        root->add_child(stars_progress_info_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(stars_progress_info_label), gameplay_ui_fabricator->get_screen_size(), 1.f);
        }
        
        std::stringstream stars_progress_str_stream;
        stars_progress_str_stream<<current_stars_count_for_rank<<"/"<<stars_for_rank;
        std::static_pointer_cast<gb::ui::textfield>(stars_progress_info_label)->set_text(stars_progress_str_stream.str());
        
        const auto rank_info_label = gameplay_ui_fabricator->create_rank_info_label("");
        root->add_child(rank_info_label);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(rank_info_label), gameplay_ui_fabricator->get_screen_size(), 1.f);
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
        
        const auto screen_overlay = gameplay_ui_fabricator->create_screen_overlay("");
        root->add_child(screen_overlay);
        
        const auto full_tickets_dialog = gameplay_ui_fabricator->create_full_tickets_dialog("");
        root->add_child(full_tickets_dialog);
        
        const auto not_enough_tickets_dialog = gameplay_ui_fabricator->create_not_enough_tickets_dialog("");
        root->add_child(not_enough_tickets_dialog);
        
#if defined(__OSX__)
        
        const auto quit_game_dialog = gameplay_ui_fabricator->create_exit_game_dialog("");
        root->add_child(quit_game_dialog);
        
#endif
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
        
        const auto next_car_in_garage_button = gameplay_ui_fabricator->create_next_car_in_garage_button("");
        root->add_child(next_car_in_garage_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(next_car_in_garage_button), gameplay_ui_fabricator->get_screen_size(), 1.f);
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
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(car_skin_1_button), gameplay_ui_fabricator->get_screen_size(), 1.f);
        }
        
        const auto car_skin_2_button = gameplay_ui_fabricator->create_car_skin_2_button("");
        root->add_child(car_skin_2_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(car_skin_2_button), gameplay_ui_fabricator->get_screen_size(), 1.f);
        }
        
        const auto car_skin_3_button = gameplay_ui_fabricator->create_car_skin_3_button("");
        root->add_child(car_skin_3_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_right(std::static_pointer_cast<gb::ui::control>(car_skin_3_button), gameplay_ui_fabricator->get_screen_size(), 1.f);
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
        
        const auto select_car_button = gameplay_ui_fabricator->create_select_car_button("");
        root->add_child(select_car_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_down(std::static_pointer_cast<gb::ui::control>(select_car_button), gameplay_ui_fabricator->get_screen_size(), 1.f);
        }
        
        std::shared_ptr<ces_garage_database_component::garage_dto::car_dto> selected_car = nullptr;
        i32 current_rank = user_database_component->get_rank(1);
        i32 claimed_rank = user_database_component->get_claimed_rank(1);
        if (current_rank != claimed_rank)
        {
            selected_car = garage_database_component->get_car(1, current_rank);
        }
        else
        {
            selected_car = garage_database_component->get_selected_car(1);
        }
        
        if (selected_car->get_id() == garage_database_component->get_selected_car(1)->get_id())
        {
            std::static_pointer_cast<gb::ui::image_button>(select_car_button)->set_image_color(glm::u8vec4(64, 64, 255, 255));
        }
        
        const auto unlock_car_button = gameplay_ui_fabricator->create_unlock_car_button("");
        root->add_child(unlock_car_button);
        if (hidden)
        {
            ui_animation_helper::hide_to_down(std::static_pointer_cast<gb::ui::control>(unlock_car_button), gameplay_ui_fabricator->get_screen_size(), 1.f);
        }
        unlock_car_button->visible = false;
        
        const auto screen_overlay = gameplay_ui_fabricator->create_screen_overlay("");
        root->add_child(screen_overlay);
        
        const auto full_tickets_dialog = gameplay_ui_fabricator->create_full_tickets_dialog("");
        root->add_child(full_tickets_dialog);
        
        const auto unlock_car_dialog = gameplay_ui_fabricator->create_unlock_car_dialog("");
        root->add_child(unlock_car_dialog);
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
}
