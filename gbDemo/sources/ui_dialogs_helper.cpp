//
//  ui_dialogs_helper.cpp
//  gbDemo
//
//  Created by serhii.s on 7/31/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ui_dialogs_helper.h"
#include "table_view.h"
#include "shop_table_view_cell.h"
#include "career_table_view_cell.h"
#include "dialog.h"
#include "button.h"
#include "image_button.h"
#include "textfield.h"
#include "progress_bar.h"
#include "ui_controls_helper.h"
#include "store_provider.h"
#include "ces_user_database_component.h"
#include "ces_ui_move_animation_action_component.h"
#include "ui_animation_helper.h"
#include "ces_levels_database_component.h"
#include "ces_scene_state_automat_component.h"
#include "ces_garage_database_component.h"

namespace game
{
    gb::ces_entity_weak_ptr ui_dialogs_helper::m_pushed_dialog;
    
    void ui_dialogs_helper::push_main_menu_button_labels()
    {
        const auto career_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_career_label);
        if (career_label)
        {
            ui_animation_helper::hide_to_up(career_label, 1.f);
            career_label->visible = true;
            auto ui_move_animation_action_component = career_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                career_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
            ui_animation_helper::move_with_animation_action(career_label);
        }
        
        const auto garage_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_garage_label);
        if (garage_label)
        {
            ui_animation_helper::hide_to_up(garage_label, 1.f);
            garage_label->visible = true;
            auto ui_move_animation_action_component = garage_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                garage_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
            ui_animation_helper::move_with_animation_action(garage_label);
        }
        
        const auto shop_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_shop_label);
        if (shop_label)
        {
            ui_animation_helper::hide_to_up(shop_label, 1.f);
            shop_label->visible = true;
            auto ui_move_animation_action_component = shop_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                shop_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
            ui_animation_helper::move_with_animation_action(shop_label);
        }
        
        const auto leaderboard_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_leaderboard_label);
        if (leaderboard_label)
        {
            ui_animation_helper::hide_to_up(leaderboard_label, 1.f);
            leaderboard_label->visible = true;
            auto ui_move_animation_action_component = leaderboard_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                leaderboard_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
            ui_animation_helper::move_with_animation_action(leaderboard_label);
        }
    }
    
    void ui_dialogs_helper::pop_main_menu_button_labels()
    {
        const auto career_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_career_label);
        if (career_label)
        {
            auto ui_move_animation_action_component = career_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                career_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
            ui_animation_helper::move_with_animation_action(career_label);
        }
        
        const auto garage_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_garage_label);
        if (garage_label)
        {
            auto ui_move_animation_action_component = garage_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                garage_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
            ui_animation_helper::move_with_animation_action(garage_label);
        }
        
        const auto shop_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_shop_label);
        if (shop_label)
        {
            auto ui_move_animation_action_component = shop_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                shop_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
            ui_animation_helper::move_with_animation_action(shop_label);
        }
        
        const auto leaderboard_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_leaderboard_label);
        if (leaderboard_label)
        {
            auto ui_move_animation_action_component = leaderboard_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                leaderboard_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
            ui_animation_helper::move_with_animation_action(leaderboard_label);
        }
    }
    
    void ui_dialogs_helper::push_shop_dialog(const gb::ces_entity_shared_ptr& root)
    {
        const auto dialog = ui_controls_helper::get_control_as<gb::ui::control>(ces_ui_interaction_component::e_ui::e_ui_shop_dialog);
        if (dialog)
        {
            m_pushed_dialog = dialog;
            ui_animation_helper::hide_to_down(dialog, 1.f);
            dialog->visible = true;
            
            pop_main_menu_button_labels();
            
            auto ui_move_animation_action_component = dialog->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                dialog->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
            ui_animation_helper::move_with_animation_action(dialog);
            
            const auto shop_table_view = std::static_pointer_cast<gb::ui::table_view>(std::static_pointer_cast<gb::ui::dialog>(dialog)->get_control(ces_ui_interaction_component::k_shop_dialog_table_view));
            
            std::vector<gb::ui::table_view_cell_data_shared_ptr> data;
            auto shop_item_cell_data = std::make_shared<ui::shop_table_view_cell_data>();
            shop_item_cell_data->id = 0;
            shop_item_cell_data->product_name = "RESTORE PRODUCTS";
            shop_item_cell_data->product_description = "RESTORE BOUGHT PRODUCTS";
            shop_item_cell_data->product_price = 0;
            shop_item_cell_data->is_bought = false;
            shop_item_cell_data->is_restore = true;
            data.push_back(shop_item_cell_data);
            
#if defined(__IOS__)
            
            shop_item_cell_data = std::make_shared<ui::shop_table_view_cell_data>();
            shop_item_cell_data->id = store_provider::k_no_ads_product_id;
            shop_item_cell_data->product_name = "NO ADS";
            shop_item_cell_data->product_description = "REMOVE BANNER AND INTERSTITIAL VIDEOS";
            shop_item_cell_data->product_price = .99f;
            shop_item_cell_data->is_bought = false;
            shop_item_cell_data->is_restore = false;
            data.push_back(shop_item_cell_data);
            
#endif
            
            shop_item_cell_data = std::make_shared<ui::shop_table_view_cell_data>();
            shop_item_cell_data->id = store_provider::k_cash_pack_1_product_id;
            shop_item_cell_data->product_name = "SMALL CASH PACK";
            shop_item_cell_data->product_description = "ADD 3000 $";
            shop_item_cell_data->product_price = .99f;
            shop_item_cell_data->is_bought = false;
            shop_item_cell_data->is_restore = false;
            data.push_back(shop_item_cell_data);
            
            shop_item_cell_data = std::make_shared<ui::shop_table_view_cell_data>();
            shop_item_cell_data->id = store_provider::k_cash_pack_2_product_id;
            shop_item_cell_data->product_name = "MEDIUM CASH PACK";
            shop_item_cell_data->product_description = "ADD 10000 $";
            shop_item_cell_data->product_price = 2.99f;
            shop_item_cell_data->is_bought = false;
            shop_item_cell_data->is_restore = false;
            data.push_back(shop_item_cell_data);
            
            shop_item_cell_data = std::make_shared<ui::shop_table_view_cell_data>();
            shop_item_cell_data->id = store_provider::k_cash_pack_3_product_id;
            shop_item_cell_data->product_name = "BIG CASH PACK";
            shop_item_cell_data->product_description = "ADD 20000 $";
            shop_item_cell_data->product_price = 4.99f;
            shop_item_cell_data->is_bought = false;
            shop_item_cell_data->is_restore = false;
            data.push_back(shop_item_cell_data);
            
            shop_table_view->set_data_source(data);
            shop_table_view->set_on_get_cell_callback([=](i32 index, const gb::ui::table_view_cell_data_shared_ptr& data, const gb::ces_entity_shared_ptr& table_view) {
                gb::ui::table_view_cell_shared_ptr cell = nullptr;
                cell = std::static_pointer_cast<gb::ui::table_view>(table_view)->reuse_cell("shop_cell", index);
                if(!cell)
                {
                    cell = gb::ces_entity::construct<ui::shop_table_view_cell>(std::static_pointer_cast<gb::ui::table_view>(table_view)->get_fabricator(),
                                                                               index, "shop_cell");
                    
                    cell->create();
                    cell->size = glm::vec2(256.f, 92.f);
                    
                    std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_buy_product_button_callback([=](i32 index) {
                        auto data_source = std::static_pointer_cast<gb::ui::table_view>(table_view)->get_data_source();
                        auto data = data_source.at(index);
                        const auto product_item_data = std::static_pointer_cast<ui::shop_table_view_cell_data>(data);
                        
                        if (product_item_data->id == store_provider::k_no_ads_product_id)
                        {
                            store_provider::shared_instance()->buy_no_ads_product();
                        }
                        
                        if (product_item_data->id == store_provider::k_cash_pack_1_product_id)
                        {
                            store_provider::shared_instance()->buy_small_cash_pack([=]() {
                                const auto user_database_component = root->get_component<ces_user_database_component>();
                                user_database_component->inc_cash(1, 3000);
                                ui_controls_helper::update_cash_amount(user_database_component->get_cash(1), 3000);
                            });
                        }
                        
                        if (product_item_data->id == store_provider::k_cash_pack_2_product_id)
                        {
                            store_provider::shared_instance()->buy_medium_cash_pack([=]() {
                                const auto user_database_component = root->get_component<ces_user_database_component>();
                                user_database_component->inc_cash(1, 10000);
                                ui_controls_helper::update_cash_amount(user_database_component->get_cash(1), 10000);
                            });
                        }
                        
                        if (product_item_data->id == store_provider::k_cash_pack_3_product_id)
                        {
                            store_provider::shared_instance()->buy_big_cash_pack([=]() {
                                const auto user_database_component = root->get_component<ces_user_database_component>();
                                user_database_component->inc_cash(1, 20000);
                                ui_controls_helper::update_cash_amount(user_database_component->get_cash(1), 20000);
                            });
                        }
                    });
                    
                    std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_restore_products_button_callback([=](i32 index) {
                        auto data_source = std::static_pointer_cast<gb::ui::table_view>(table_view)->get_data_source();
                        auto data = data_source.at(index);
                        const auto product_item_data = std::static_pointer_cast<ui::shop_table_view_cell_data>(data);
                        store_provider::shared_instance()->restore_products();
                        pop_dialog();
                    });
                }
                
                const auto product_item_data = std::static_pointer_cast<ui::shop_table_view_cell_data>(data);
                std::string name = product_item_data->product_name;
                std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_product_name(name);
                std::string description = product_item_data->product_description;
                std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_product_description(description);
                f32 price = product_item_data->product_price;
                std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_product_price(price);
                bool is_bought = product_item_data->is_bought;
                std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_is_bought(is_bought);
                bool is_restore = product_item_data->is_restore;
                std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_is_restore(is_restore);
                
                return cell;
            });
            shop_table_view->set_on_get_table_cell_height_callback([](i32 index) {
                return 96.f;
            });
            shop_table_view->reload_data();
        }
    }
    
    void ui_dialogs_helper::pop_shop_dialog()
    {
        const auto dialog = ui_controls_helper::get_control_as<gb::ui::control>(ces_ui_interaction_component::e_ui::e_ui_shop_dialog);
        if (dialog)
        {
            push_main_menu_button_labels();
            
            auto ui_move_animation_action_component = dialog->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                dialog->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
            ui_animation_helper::move_with_animation_action(dialog);
        }
    }
    
    void ui_dialogs_helper::push_career_dialog(const gb::ces_entity_shared_ptr& root)
    {
        const auto dialog = ui_controls_helper::get_control_as<gb::ui::control>(ces_ui_interaction_component::e_ui::e_ui_career_dialog);
        if (dialog)
        {
            m_pushed_dialog = dialog;
            ui_animation_helper::hide_to_down(dialog, 1.f);
            dialog->visible = true;
            
            pop_main_menu_button_labels();
            
            auto ui_move_animation_action_component = dialog->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                dialog->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
            ui_animation_helper::move_with_animation_action(dialog);
            
            const auto levels_database_component = root->get_component<ces_levels_database_component>();
            const auto levels = levels_database_component->get_all_levels();
            
            const auto career_table_view = std::static_pointer_cast<gb::ui::table_view>(std::static_pointer_cast<gb::ui::dialog>(dialog)->get_control(ces_ui_interaction_component::k_career_dialog_table_view));
            
            std::vector<gb::ui::table_view_cell_data_shared_ptr> data;
            for (i32 i = 1; i <= levels.size(); ++i)
            {
                auto level_data_it = levels.find(i);
                if (level_data_it != levels.end())
                {
                    auto career_cell_data = std::make_shared<ui::career_table_view_cell_data>();
                    career_cell_data->id = level_data_it->second->get_id();
                    bool is_locked = !level_data_it->second->get_is_openned();
                    career_cell_data->is_locked = is_locked;
                    bool is_passed = level_data_it->second->get_is_passed();
                    career_cell_data->is_passed = is_passed;
                    i32 stars_count = 0;
                    if (level_data_it->second->get_is_star_1_received())
                    {
                        stars_count++;
                    }
                    if (level_data_it->second->get_is_star_2_received())
                    {
                        stars_count++;
                    }
                    if (level_data_it->second->get_is_star_3_received())
                    {
                        stars_count++;
                    }
                    career_cell_data->stars_count = stars_count;
                    career_cell_data->drift_time = level_data_it->second->get_drift_time();
                    data.push_back(career_cell_data);
                }
            }
            career_table_view->set_data_source(data);
            career_table_view->set_on_get_cell_callback([=](i32 index, const gb::ui::table_view_cell_data_shared_ptr& data, const gb::ces_entity_shared_ptr& table_view) {
                gb::ui::table_view_cell_shared_ptr cell = nullptr;
                cell = std::static_pointer_cast<gb::ui::table_view>(table_view)->reuse_cell("career_cell", index);
                if(!cell)
                {
                    cell = gb::ces_entity::construct<ui::career_table_view_cell>(std::static_pointer_cast<gb::ui::table_view>(table_view)->get_fabricator(),
                                                                                 index, "career_cell");
                    
                    cell->create();
                    cell->size = glm::vec2(256.f, 92.f);
                    
                    std::static_pointer_cast<ui::career_table_view_cell>(cell)->set_start_level_button_callback_t([=](i32 index) {
                        auto data_source = std::static_pointer_cast<gb::ui::table_view>(table_view)->get_data_source();
                        auto data = data_source.at(index);
                        const auto career_cell_data = std::static_pointer_cast<ui::career_table_view_cell_data>(data);
                        
                        levels_database_component->set_playing_level_id(career_cell_data->id);
                        root->get_component<ces_scene_state_automat_component>()->mode = ces_scene_state_automat_component::e_mode_in_game;
                        root->get_component<ces_scene_state_automat_component>()->state = ces_scene_state_automat_component::e_state_should_preload;
                        pop_dialog();
                    });
                }
                
                const auto career_cell_data = std::static_pointer_cast<ui::career_table_view_cell_data>(data);
                std::static_pointer_cast<ui::career_table_view_cell>(cell)->set_index(career_cell_data->id);
                bool is_locked = career_cell_data->is_locked;
                std::static_pointer_cast<ui::career_table_view_cell>(cell)->set_is_locked(is_locked);
                bool is_passed = career_cell_data->is_passed;
                std::static_pointer_cast<ui::career_table_view_cell>(cell)->set_is_passed(is_passed);
                i32 stars_count = career_cell_data->stars_count;
                std::static_pointer_cast<ui::career_table_view_cell>(cell)->set_stars_count(stars_count);
                f32 drift_time = career_cell_data->drift_time;
                std::static_pointer_cast<ui::career_table_view_cell>(cell)->set_drift_time(drift_time);
                
                return cell;
            });
            career_table_view->set_on_get_table_cell_height_callback([](i32 index) {
                return 96.f;
            });
            career_table_view->reload_data();
        }
    }
    
    void ui_dialogs_helper::pop_career_dialog()
    {
        const auto dialog = ui_controls_helper::get_control_as<gb::ui::control>(ces_ui_interaction_component::e_ui::e_ui_career_dialog);
        if (dialog)
        {
            push_main_menu_button_labels();
            
            auto ui_move_animation_action_component = dialog->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                dialog->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
            ui_animation_helper::move_with_animation_action(dialog);
        }
    }
    
    void ui_dialogs_helper::push_quit_game_dialog(const gb::ces_entity_shared_ptr& root)
    {
        const auto dialog = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_quit_game_dialog);
        if (dialog)
        {
            m_pushed_dialog = dialog;
            dialog->visible = true;
            
            ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_screen_overlay)->visible = true;
            ui_controls_helper::disable_all_and_focus_on({ces_ui_interaction_component::e_ui::e_ui_quit_game_dialog}, false);
            
            const auto yes_button = std::static_pointer_cast<gb::ui::image_button>(std::static_pointer_cast<gb::ui::dialog>(dialog)->get_control(ces_ui_interaction_component::k_quit_dialog_yes_button));
            
            if(!yes_button->is_pressed_callback_exist())
            {
                yes_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                    exit(0);
                });
            }
            
            const auto no_button = std::static_pointer_cast<gb::ui::image_button>(std::static_pointer_cast<gb::ui::dialog>(dialog)->get_control(ces_ui_interaction_component::k_quit_dialog_no_button));
            
            if(!no_button->is_pressed_callback_exist())
            {
                no_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                    pop_dialog();
                });
            }
        }
    }
    
    void ui_dialogs_helper::pop_quit_game_dialog()
    {
        const auto dialog = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_quit_game_dialog);
        if (dialog)
        {
            dialog->visible = false;
            ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_screen_overlay)->visible = false;
            ui_controls_helper::enable_all_and_unfocus();
        }
    }
    
    void ui_dialogs_helper::push_cash_shop_dialog(const gb::ces_entity_shared_ptr& root)
    {
        const auto dialog = ui_controls_helper::get_control_as<gb::ces_entity>(ces_ui_interaction_component::e_ui::e_ui_cash_shop_dialog);
        if (dialog)
        {
            dialog->visible = true;
            m_pushed_dialog = dialog;
            
            ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_screen_overlay)->visible = true;
            ui_controls_helper::disable_all_and_focus_on({ces_ui_interaction_component::e_ui::e_ui_cash_shop_dialog}, false);
            
            const auto table_view = std::static_pointer_cast<gb::ui::table_view>(std::static_pointer_cast<gb::ui::dialog>(dialog)->get_control(ces_ui_interaction_component::k_cash_shop_dialog_table_view));
            
            std::vector<gb::ui::table_view_cell_data_shared_ptr> data;
            auto shop_item_cell_data = std::make_shared<ui::shop_table_view_cell_data>();
            shop_item_cell_data->id = store_provider::k_cash_pack_1_product_id;
            shop_item_cell_data->product_name = "SMALL CASH PACK";
            shop_item_cell_data->product_description = "ADD 3000 $";
            shop_item_cell_data->product_price = .99f;
            shop_item_cell_data->is_bought = false;
            shop_item_cell_data->is_restore = false;
            data.push_back(shop_item_cell_data);
            
            shop_item_cell_data = std::make_shared<ui::shop_table_view_cell_data>();
            shop_item_cell_data->id = store_provider::k_cash_pack_2_product_id;
            shop_item_cell_data->product_name = "MEDIUM CASH PACK";
            shop_item_cell_data->product_description = "ADD 10000 $";
            shop_item_cell_data->product_price = 2.99;
            shop_item_cell_data->is_bought = false;
            shop_item_cell_data->is_restore = false;
            data.push_back(shop_item_cell_data);
            
            shop_item_cell_data = std::make_shared<ui::shop_table_view_cell_data>();
            shop_item_cell_data->id = store_provider::k_cash_pack_3_product_id;
            shop_item_cell_data->product_name = "BIG CASH PACK";
            shop_item_cell_data->product_description = "ADD 20000 $";
            shop_item_cell_data->product_price = 4.99;
            shop_item_cell_data->is_bought = false;
            shop_item_cell_data->is_restore = false;
            data.push_back(shop_item_cell_data);
            
            table_view->set_data_source(data);
            table_view->set_on_get_cell_callback([=](i32 index, const gb::ui::table_view_cell_data_shared_ptr& data, const gb::ces_entity_shared_ptr& table_view) {
                gb::ui::table_view_cell_shared_ptr cell = nullptr;
                cell = std::static_pointer_cast<gb::ui::table_view>(table_view)->reuse_cell("shop_cell", index);
                if(!cell)
                {
                    cell = gb::ces_entity::construct<ui::shop_table_view_cell>(std::static_pointer_cast<gb::ui::table_view>(table_view)->get_fabricator(),
                                                                               index, "shop_cell");
                    
                    cell->create();
                    cell->size = glm::vec2(256.f, 92.f);
                    
                    std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_buy_product_button_callback([=](i32 index) {
                        auto data_source = std::static_pointer_cast<gb::ui::table_view>(table_view)->get_data_source();
                        auto data = data_source.at(index);
                        const auto product_item_data = std::static_pointer_cast<ui::shop_table_view_cell_data>(data);
                        if (product_item_data->id == store_provider::k_cash_pack_1_product_id)
                        {
                            store_provider::shared_instance()->buy_small_cash_pack([=](){
                                const auto user_database_component = root->get_component<ces_user_database_component>();
                                user_database_component->inc_cash(1, 3000);
                                ui_controls_helper::update_cash_amount(user_database_component->get_cash(1), 3000);
                            });
                        }
                        
                        if (product_item_data->id == store_provider::k_cash_pack_2_product_id)
                        {
                            store_provider::shared_instance()->buy_medium_cash_pack([=](){
                                const auto user_database_component = root->get_component<ces_user_database_component>();
                                user_database_component->inc_cash(1, 10000);
                                ui_controls_helper::update_cash_amount(user_database_component->get_cash(1), 10000);
                            });
                        }
                        
                        if (product_item_data->id == store_provider::k_cash_pack_3_product_id)
                        {
                            store_provider::shared_instance()->buy_big_cash_pack([=](){
                                const auto user_database_component = root->get_component<ces_user_database_component>();
                                user_database_component->inc_cash(1, 20000);
                                ui_controls_helper::update_cash_amount(user_database_component->get_cash(1), 20000);
                            });
                        }
                        
                        pop_dialog();
                    });
                }
                
                const auto product_item_data = std::static_pointer_cast<ui::shop_table_view_cell_data>(data);
                std::string name = product_item_data->product_name;
                std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_product_name(name);
                std::string description = product_item_data->product_description;
                std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_product_description(description);
                f32 price = product_item_data->product_price;
                std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_product_price(price);
                bool is_bought = product_item_data->is_bought;
                std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_is_bought(is_bought);
                bool is_restore = product_item_data->is_restore;
                std::static_pointer_cast<ui::shop_table_view_cell>(cell)->set_is_restore(is_restore);
                
                return cell;
            });
            table_view->set_on_get_table_cell_height_callback([](i32 index) {
                return 96.f;
            });
            table_view->reload_data();
            
            const auto ok_button = std::static_pointer_cast<gb::ui::image_button>(std::static_pointer_cast<gb::ui::dialog>(dialog)->get_control(ces_ui_interaction_component::k_cash_shop_ok_button));
            
            if(ok_button && !ok_button->is_pressed_callback_exist())
            {
                ok_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                    pop_dialog();
                });
            }
        }
    }
    
    void ui_dialogs_helper::pop_cash_shop_dialog()
    {
        const auto dialog = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_cash_shop_dialog);
        if (dialog)
        {
            dialog->visible = false;
            ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_screen_overlay)->visible = false;
            ui_controls_helper::enable_all_and_unfocus();
        }
    }
    
    void ui_dialogs_helper::push_garage_button_labels()
    {
        const auto body_paint_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_body_paint_label);
        if (body_paint_label)
        {
            ui_animation_helper::hide_to_up(body_paint_label, 1.f);
            body_paint_label->visible = true;
            auto ui_move_animation_action_component = body_paint_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                body_paint_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
            ui_animation_helper::move_with_animation_action(body_paint_label);
        }
        
        const auto windshield_paint_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_windshield_paint_label);
        if (windshield_paint_label)
        {
            ui_animation_helper::hide_to_up(windshield_paint_label, 1.f);
            windshield_paint_label->visible = true;
            auto ui_move_animation_action_component = windshield_paint_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                windshield_paint_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
            ui_animation_helper::move_with_animation_action(windshield_paint_label);
        }
        
        const auto performance_upgrade_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_performance_label);
        if (performance_upgrade_label)
        {
            ui_animation_helper::hide_to_up(performance_upgrade_label, 1.f);
            performance_upgrade_label->visible = true;
            auto ui_move_animation_action_component = performance_upgrade_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                performance_upgrade_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
            ui_animation_helper::move_with_animation_action(performance_upgrade_label);
        }
    }
    
    void ui_dialogs_helper::pop_garage_button_labels()
    {
        const auto body_paint_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_body_paint_label);
        if (body_paint_label)
        {
            auto ui_move_animation_action_component = body_paint_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                body_paint_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
            ui_animation_helper::move_with_animation_action(body_paint_label);
        }
        
        const auto windshield_paint_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_windshield_paint_label);
        if (windshield_paint_label)
        {
            auto ui_move_animation_action_component = windshield_paint_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                windshield_paint_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
            ui_animation_helper::move_with_animation_action(windshield_paint_label);
        }
        
        const auto performance_upgrade_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_performance_label);
        if (performance_upgrade_label)
        {
            auto ui_move_animation_action_component = performance_upgrade_label->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                performance_upgrade_label->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
            ui_animation_helper::move_with_animation_action(performance_upgrade_label);
        }
    }
    
    void ui_dialogs_helper::push_body_paint_dialog(const gb::ces_entity_shared_ptr& root)
    {
        const auto dialog = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_body_paint_dialog);
        if (dialog)
        {
            m_pushed_dialog = dialog;
            ui_animation_helper::hide_to_down(dialog, 1.f);
            dialog->visible = true;
            pop_garage_button_labels();
            
            auto ui_move_animation_action_component = dialog->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                dialog->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
            ui_animation_helper::move_with_animation_action(dialog);
        }
    }
    
    void ui_dialogs_helper::pop_body_paint_dialog()
    {
        const auto dialog = ui_controls_helper::get_control_as<gb::ui::control>(ces_ui_interaction_component::e_ui::e_ui_body_paint_dialog);
        if (dialog)
        {
            push_garage_button_labels();
            
            auto ui_move_animation_action_component = dialog->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                dialog->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
            ui_animation_helper::move_with_animation_action(dialog);
        }
    }
    
    void ui_dialogs_helper::push_windshield_paint_dialog(const gb::ces_entity_shared_ptr& root)
    {
        const auto dialog = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_windshield_paint_dialog);
        if (dialog)
        {
            m_pushed_dialog = dialog;
            ui_animation_helper::hide_to_down(dialog, 1.f);
            dialog->visible = true;
            pop_garage_button_labels();
            
            auto ui_move_animation_action_component = dialog->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                dialog->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
            ui_animation_helper::move_with_animation_action(dialog);
        }
    }
    
    void ui_dialogs_helper::pop_windshield_paint_dialog()
    {
        const auto dialog = ui_controls_helper::get_control_as<gb::ui::control>(ces_ui_interaction_component::e_ui::e_ui_windshield_paint_dialog);
        if (dialog)
        {
            push_garage_button_labels();
            
            auto ui_move_animation_action_component = dialog->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                dialog->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
            ui_animation_helper::move_with_animation_action(dialog);
        }
    }
    
    void ui_dialogs_helper::push_performance_upgrade_dialog(const gb::ces_entity_shared_ptr& root)
    {
        const auto dialog = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_performance_upgrade_dialog);
        if (dialog)
        {
            m_pushed_dialog = dialog;
            ui_animation_helper::hide_to_down(dialog, 1.f);
            dialog->visible = true;
            pop_garage_button_labels();
            
            auto ui_move_animation_action_component = dialog->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                dialog->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
            ui_animation_helper::move_with_animation_action(dialog);
        }
    }
    
    void ui_dialogs_helper::pop_performance_upgrade_dialog()
    {
        const auto dialog = ui_controls_helper::get_control_as<gb::ui::control>(ces_ui_interaction_component::e_ui::e_ui_performance_upgrade_dialog);
        if (dialog)
        {
            push_garage_button_labels();
            
            auto ui_move_animation_action_component = dialog->get_component<ces_ui_move_animation_action_component>();
            if (!ui_move_animation_action_component)
            {
                ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                dialog->add_component(ui_move_animation_action_component);
            }
            ui_move_animation_action_component->set_duration_in_second(.33f);
            ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
            ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
            ui_animation_helper::move_with_animation_action(dialog);
        }
    }
    
    void ui_dialogs_helper::push_unlock_car_dialog(const gb::ces_entity_shared_ptr& root)
    {
        const auto dialog = ui_controls_helper::get_control_as<gb::ces_entity>(ces_ui_interaction_component::e_ui::e_ui_unlock_car_dialog);
        if (dialog)
        {
            dialog->visible = true;
            m_pushed_dialog = dialog;
            
            ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_screen_overlay)->visible = true;
            ui_controls_helper::disable_all_and_focus_on({ces_ui_interaction_component::e_ui::e_ui_unlock_car_dialog}, false);
            
            const auto ok_button = std::static_pointer_cast<gb::ui::image_button>(std::static_pointer_cast<gb::ui::dialog>(dialog)->get_control(ces_ui_interaction_component::k_car_unlock_dialog_ok_button));
            
            if(!ok_button->is_pressed_callback_exist())
            {
                ok_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                    pop_dialog();
                });
            }
            
            const auto garage_database_component = root->get_component<ces_garage_database_component>();
            auto selected_car_id = garage_database_component->get_previewed_car_id();
            i32 unlocked_at_rank = garage_database_component->get_car_unlocked_rank(selected_car_id);
            std::stringstream message_str_stream;
            message_str_stream<<"CAR WILL BE UNLOCKED AT RANK "<<unlocked_at_rank;
            
            const auto label = std::static_pointer_cast<gb::ui::textfield>(std::static_pointer_cast<gb::ui::dialog>(dialog)->get_control(ces_ui_interaction_component::k_car_unlock_dialog_label));
            label->set_text(message_str_stream.str());
        }
    }
    
    void ui_dialogs_helper::pop_unlock_car_dialog()
    {
        const auto dialog = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_unlock_car_dialog);
        if (dialog)
        {
            dialog->visible = false;
            ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_screen_overlay)->visible = false;
            ui_controls_helper::enable_all_and_unfocus();
        }
    }
    
    void ui_dialogs_helper::push_dialog(ces_ui_interaction_component::e_ui ui_id, const gb::ces_entity_shared_ptr& root)
    {
        switch (ui_id)
        {
            case ces_ui_interaction_component::e_ui::e_ui_shop_dialog:
            {
                push_shop_dialog(root);
            }
            break;
            
            case ces_ui_interaction_component::e_ui::e_ui_career_dialog:
            {
                push_career_dialog(root);
            }
            break;
            case ces_ui_interaction_component::e_ui::e_ui_quit_game_dialog:
            {
                push_quit_game_dialog(root);
            }
            break;
            case ces_ui_interaction_component::e_ui::e_ui_cash_shop_dialog:
            {
                push_cash_shop_dialog(root);
            }
            break;
            case ces_ui_interaction_component::e_ui::e_ui_body_paint_dialog:
            {
                push_body_paint_dialog(root);
            }
            break;
            case ces_ui_interaction_component::e_ui::e_ui_windshield_paint_dialog:
            {
                push_windshield_paint_dialog(root);
            }
            break;
            case ces_ui_interaction_component::e_ui::e_ui_performance_upgrade_dialog:
            {
                push_performance_upgrade_dialog(root);
            }
            break;
            case ces_ui_interaction_component::e_ui::e_ui_unlock_car_dialog:
            {
                push_unlock_car_dialog(root);
            }
                break;
            default:
            assert(false);
            break;
        }
    }
    
    void ui_dialogs_helper::pop_dialog()
    {
        if (!m_pushed_dialog.expired())
        {
            const auto ui_interaction_component = m_pushed_dialog.lock()->get_component<ces_ui_interaction_component>();
            switch (ui_interaction_component->get_ui())
            {
                case ces_ui_interaction_component::e_ui::e_ui_shop_dialog:
                {
                    pop_shop_dialog();
                }
                break;
                case ces_ui_interaction_component::e_ui::e_ui_career_dialog:
                {
                    pop_career_dialog();
                }
                break;
                case ces_ui_interaction_component::e_ui::e_ui_quit_game_dialog:
                {
                    pop_quit_game_dialog();
                }
                break;
                case ces_ui_interaction_component::e_ui::e_ui_cash_shop_dialog:
                {
                    pop_cash_shop_dialog();
                }
                break;
                case ces_ui_interaction_component::e_ui::e_ui_body_paint_dialog:
                {
                    pop_body_paint_dialog();
                }
                break;
                case ces_ui_interaction_component::e_ui::e_ui_windshield_paint_dialog:
                {
                    pop_windshield_paint_dialog();
                }
                break;
                case ces_ui_interaction_component::e_ui::e_ui_performance_upgrade_dialog:
                {
                    pop_performance_upgrade_dialog();
                }
                break;
                case ces_ui_interaction_component::e_ui::e_ui_unlock_car_dialog:
                {
                    pop_unlock_car_dialog();
                }
                    break;
                default:
                assert(false);
                break;
            }
            m_pushed_dialog.reset();
        }
    }
    
    gb::ces_entity_shared_ptr ui_dialogs_helper::get_pushed_dialog()
    {
        return m_pushed_dialog.lock();
    }
}
