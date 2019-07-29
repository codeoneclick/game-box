//
//  ces_state_automat_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/25/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_state_automat_system.h"
#include "ces_scene_state_automat_component.h"
#include "ces_scene_visual_effects_component.h"
#include "ces_level_descriptor_component.h"
#include "ces_car_input_component.h"
#include "ces_car_ai_input_component.h"
#include "ces_system_modifiers_component.h"
#include "ces_car_parts_component.h"
#include "ces_ui_interaction_component.h"
#include "ces_scene_fabricator_component.h"
#include "ces_car_camera_follow_component.h"
#include "ces_box2d_world_component.h"
#include "ces_level_route_component.h"
#include "ces_car_simulator_system.h"
#include "ces_interaction_system.h"
#include "ces_ai_system.h"
#include "ces_box2d_system.h"
#include "ces_car_sound_system.h"
#include "gameplay_fabricator.h"
#include "gameplay_ui_fabricator.h"
#include "game_object_3d.h"
#include "camera_3d.h"
#include "button.h"
#include "textfield.h"
#include "image_button.h"
#include "ces_garage_database_component.h"
#include "ces_car_drift_state_component.h"
#include "ces_level_tutorial_component.h"
#include "ces_sound_component.h"
#include "ces_car_sounds_set_component.h"
#include "progress_bar.h"
#include "ui_animation_helper.h"
#include "ui_controls_helper.h"
#include "ces_user_database_component.h"
#include "ces_car_descriptor_component.h"
#include "ces_levels_database_component.h"
#include "events_provider.h"
#include "ces_ui_interaction_system.h"
#include "ces_box2d_body_component.h"
#include "ui_menus_helper.h"

namespace game
{
    ces_state_automat_system::ces_state_automat_system()
    {
        ces_base_system::add_required_component_guid(m_state_automat_components_mask, ces_scene_state_automat_component::class_guid());
        ces_base_system::add_required_components_mask(m_state_automat_components_mask);
        
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_descriptor_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_main_car_components_mask, ces_car_input_component::class_guid());
        ces_base_system::add_required_components_mask(m_main_car_components_mask);
        
        ces_base_system::add_required_component_guid(m_ai_car_components_mask, ces_car_ai_input_component::class_guid());
        ces_base_system::add_required_components_mask(m_ai_car_components_mask);
        
        ces_base_system::add_required_component_guid(m_ui_components_mask, ces_ui_interaction_component::class_guid());
        ces_base_system::add_required_components_mask(m_ui_components_mask);
    }
    
    ces_state_automat_system::~ces_state_automat_system()
    {
        
    }
    
    void ces_state_automat_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_state_automat_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_state_automat_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            const auto scene_state_automat_component = entity->get_component<ces_scene_state_automat_component>();
            const auto scene_visual_effects_component = entity->get_component<ces_scene_visual_effects_component>();
            const auto system_modifiers_component = entity->get_component<gb::ces_system_modifiers_component>();
            const auto scene_fabricator_component = entity->get_component<ces_scene_fabricator_component>();
            const auto user_database_component = entity->get_component<ces_user_database_component>();
            const auto levels_database_component = entity->get_component<ces_levels_database_component>();
            
            i32 last_ticket_dec_timestamp = user_database_component->get_last_ticket_dec_timestamp(1);
            if (last_ticket_dec_timestamp != 0)
            {
                i32 max_time_interval_for_ticket_generation = user_database_component->get_max_time_interval_for_ticket_generation();
                i32 current_time = static_cast<i32>(std::get_tick_count());
                i32 delta_time = current_time - last_ticket_dec_timestamp;
                if (delta_time < 0)
                {
                    i32 current_tickets_count = user_database_component->get_tickets(1);
                    current_tickets_count += 1;
                    user_database_component->update_tickets(1, std::min(current_tickets_count, 5));
                }
                if (delta_time >= max_time_interval_for_ticket_generation)
                {
                    i32 current_tickets_count = user_database_component->get_tickets(1);
                    current_tickets_count += delta_time / max_time_interval_for_ticket_generation;
                    user_database_component->update_tickets(1, std::min(current_tickets_count, 5));
                }
                else
                {
                    if (ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_tickets_label))
                    {
                        i32 minutes = 0;
                        i32 seconds = (max_time_interval_for_ticket_generation - delta_time) / 1000;
                        if (seconds >= 60)
                        {
                            minutes = seconds / 60;
                            seconds = seconds % (minutes * 60);
                        }
                        i32 current_tickets_count = user_database_component->get_tickets(1);
                        std::stringstream tickets_value_string_stream;
                        tickets_value_string_stream<<"TICKETS ("<<(minutes < 10 ? "0" : "")<<minutes<<":"<<(seconds < 10 ? "0" : "")<<seconds<<"): "<<current_tickets_count;
                        ui_controls_helper::get_control_as<gb::ui::textfield>(ces_ui_interaction_component::e_ui::e_ui_tickets_label)->set_text(tickets_value_string_stream.str());
                    }
                }
               
            }
            else
            {
                if (ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_tickets_label))
                {
                    i32 current_tickets_count = user_database_component->get_tickets(1);
                    std::stringstream tickets_value_string_stream;
                    tickets_value_string_stream<<"TICKETS: "<<current_tickets_count;
                    ui_controls_helper::get_control_as<gb::ui::textfield>(ces_ui_interaction_component::e_ui::e_ui_tickets_label)->set_text(tickets_value_string_stream.str());
                }
            }
            
            const gameplay_fabricator_shared_ptr gameplay_fabricator = scene_fabricator_component->gameplay_fabricator;
            const gameplay_ui_fabricator_shared_ptr gameplay_ui_fabricator = scene_fabricator_component->gameplay_ui_fabricator;
            
            if (scene_state_automat_component->state == ces_scene_state_automat_component::e_state_none)
            {
                if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_main_menu)
                {
                    scene_visual_effects_component->is_noises_enabled = true;
                    root->get_component<gb::ces_box2d_world_component>()->set_update_interval(1.f);
                    
                    i32 rank = user_database_component->get_rank(1);
                    i32 claimed_rank = user_database_component->get_claimed_rank(1);
                    if (rank != claimed_rank)
                    {
                        const auto stars_progress_button = ui_controls_helper::get_control_as<gb::ui::image_button>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_button);
                        if (stars_progress_button)
                        {
                            static i64 claim_rank_blink_timestamp = 0;
                            static i32 blink_state = 0;
                            i64 current_time = std::get_tick_count();
                            if (current_time - claim_rank_blink_timestamp > 333)
                            {
                                if (blink_state == 0)
                                {
                                    blink_state = 1;
                                    stars_progress_button->set_image_color(glm::u8vec4(255, 0, 255, 255));
                                }
                                else
                                {
                                    blink_state = 0;
                                    stars_progress_button->set_image_color(glm::u8vec4(255, 255, 255, 255));
                                }
                                claim_rank_blink_timestamp = current_time;
                            }
                        }
                    }
                }
                if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_in_game)
                {
                   
                }
                if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_garage)
                {
                    scene_visual_effects_component->is_noises_enabled = false;
                    root->get_component<gb::ces_box2d_world_component>()->set_update_interval(.25f);
                }
                else if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_in_game)
                {
                    if (!m_level.expired())
                    {
                        const auto level_descriptor_component = m_level.lock()->get_component<ces_level_descriptor_component>();
                        if (level_descriptor_component->is_started &&
                            !level_descriptor_component->is_paused &&
                            !level_descriptor_component->is_win &&
                            !level_descriptor_component->is_loose)
                        {
                            f32 start_timestamp = level_descriptor_component->start_timestamp;
                            f32 delta = std::get_tick_count() - start_timestamp;
                            delta /= 1000.f;
                            f32 countdown_time = level_descriptor_component->countdown_time;
                            f32 current_countdown_time = countdown_time - delta;
                            level_descriptor_component->current_countdown_time = current_countdown_time;
                            
                            if (current_countdown_time <= 0.f)
                            {
                                system_modifiers_component->pause_system(ces_car_simulator_system::class_guid(), false);
                                system_modifiers_component->pause_system(ces_interaction_system::class_guid(), false);
                                system_modifiers_component->pause_system(ces_ai_system::class_guid(), false);
                                system_modifiers_component->pause_system(gb::ces_box2d_system::class_guid(), false);
                                system_modifiers_component->pause_system(ces_car_sound_system::class_guid(), false);
                                
                                scene_visual_effects_component->is_noises_enabled = false;
                                
                                if (!m_main_car.expired())
                                {
                                    const auto car_parts_component = m_main_car.lock()->get_component<ces_car_parts_component>();
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_label)->visible = true;
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_value_label)->visible = true;
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_label)->visible = true;
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_value_label)->visible = true;
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_label)->visible = true;
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_value_label)->visible = true;
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_direction_arrow)->visible = true;
                                }
                                
                                f32 round_time_delta = level_descriptor_component->round_time_delta;
                                round_time_delta += dt;
                                level_descriptor_component->round_time_delta = round_time_delta;
                            }
                        }
                        else if (level_descriptor_component->is_paused)
                        {
                            system_modifiers_component->pause_system(ces_car_simulator_system::class_guid(), true);
                            system_modifiers_component->pause_system(ces_interaction_system::class_guid(), true);
                            system_modifiers_component->pause_system(ces_ai_system::class_guid(), true);
                            system_modifiers_component->pause_system(gb::ces_box2d_system::class_guid(), true);
                            system_modifiers_component->pause_system(ces_car_sound_system::class_guid(), true);
                            
                            scene_visual_effects_component->is_noises_enabled = true;
                            
                            const auto sound_component = m_main_car.lock()->get_component<gb::al::ces_sound_component>();
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_idle, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_low, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_mid, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_high, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_low, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_mid, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_high, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_drift, 0.f);
                        }
                        else if (level_descriptor_component->is_win || level_descriptor_component->is_loose)
                        {
                            scene_visual_effects_component->is_noises_enabled = true;
                            
                            const auto sound_component = m_main_car.lock()->get_component<gb::al::ces_sound_component>();
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_idle, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_low, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_mid, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_high, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_low, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_mid, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_high, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_drift, 0.f);
                        }
                    }
                }
            }
            else if (scene_state_automat_component->state == ces_scene_state_automat_component::e_state_should_preload)
            {
                scene_state_automat_component->state = ces_scene_state_automat_component::e_state_pre_loading;
                scene_visual_effects_component->is_crossfade_enabled = true;
                scene_visual_effects_component->crossfade_progress = 0.f;
            }
            else if (scene_state_automat_component->state == ces_scene_state_automat_component::e_state_pre_loading)
            {
                f32 loading_progress = scene_state_automat_component->loading_progress;
                scene_visual_effects_component->is_crossfade_enabled = true;
                scene_visual_effects_component->crossfade_progress = loading_progress;
                scene_visual_effects_component->is_noises_enabled = false;
                f32 hide_progress = 1.f - loading_progress;
                
                if (loading_progress <= 1.f)
                {
                    loading_progress += ces_scene_state_automat_component::k_loading_progress_inc_value;
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_open_levels_list_dialog_button),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_career_label),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_goto_racing1_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_garage_label),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_goto_racing2_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_open_garage_button),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_back_from_garage_button),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_back_from_garage_label),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_garage_preview_button),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_body_color_1_button),
                                                      gameplay_ui_fabricator->get_screen_size(),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_body_color_2_button),
                                                        gameplay_ui_fabricator->get_screen_size(),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_body_color_3_button),
                                                        gameplay_ui_fabricator->get_screen_size(),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_body_color_4_button),
                                                        gameplay_ui_fabricator->get_screen_size(),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_windows_color_1_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_windows_color_2_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_windows_color_3_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_windows_color_4_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_body_paint_label),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_windshield_paint_label),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_performance_label),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_label),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_progress_bar),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_plus_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_label),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_progress_bar),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_plus_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_rigidity_label),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_rigidity_progress_bar),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_rigidity_plus_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_buy_upgrade_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_buy_upgrade_label),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_open_shop_button),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_shop_label),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_quit_game_button),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_facebook_button),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_next_car_in_garage_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_prev_car_in_garage_button),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_car_skin_1_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_car_skin_2_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_car_skin_3_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_tickets_label),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_cash_label),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_down(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_select_car_button),
                                                      gameplay_ui_fabricator->get_screen_size(),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_down(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_unlock_car_button),
                                                      gameplay_ui_fabricator->get_screen_size(),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_buy_car_button),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_buy_car_label),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_stars_progress_bar),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_stars_progress_label),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_stars_progress_info_label),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                    
                    ui_animation_helper::hide_to_up(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_stars_progress_button),
                                                      hide_progress);
                    
                    ui_animation_helper::hide_to_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_rank_info_label),
                                                       gameplay_ui_fabricator->get_screen_size(),
                                                       hide_progress);
                }
                else
                {
                    loading_progress = 1.f;
                    ui_controls_helper::clear_controls();
                    
                    const auto sound_component = root->get_component<gb::al::ces_sound_component>();
                    sound_component->trigger_sound("music_01.mp3", true);
                    sound_component->trigger_sound("music_05.mp3", true);
                    sound_component->trigger_sound("music_03.mp3", true);
                    
                    if (!m_level.expired())
                    {
                        m_level.lock()->remove_from_parent();
                        m_level.reset();
                    }
                    
                    if(!m_main_car.expired())
                    {
                        m_main_car.lock()->remove_from_parent();
                        m_main_car.reset();
                    }
                    
                    for (auto ai_car_it : m_all_cars)
                    {
                        if (!ai_car_it.second.expired())
                        {
                            ai_car_it.second.lock()->remove_from_parent();
                            ai_car_it.second.reset();
                        }
                    }
                    m_all_cars.clear();
                    system_modifiers_component->cleanup();
                    
                    if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_main_menu)
                    {
                        system_modifiers_component->pause_system(ces_car_simulator_system::class_guid(), false);
                        system_modifiers_component->pause_system(ces_interaction_system::class_guid(), false);
                        system_modifiers_component->pause_system(ces_ai_system::class_guid(), false);
                        system_modifiers_component->pause_system(gb::ces_box2d_system::class_guid(), false);
                        system_modifiers_component->pause_system(ces_car_sound_system::class_guid(), false);
                        
                        i32 next_level_id = levels_database_component->get_next_level_id();
                        const auto level_data = levels_database_component->get_level(next_level_id);
                        const auto level = gameplay_fabricator->create_scene(level_data->get_scene_filename());
                        root->add_child(level);
                        
                        const auto garage_database_component = root->get_component<ces_garage_database_component>();
                        const auto selected_car = garage_database_component->get_selected_car(1);
                        
                        std::stringstream selected_car_configuration_filename;
                        selected_car_configuration_filename<<"car_0";
                        selected_car_configuration_filename<<selected_car->get_id();
                        const auto main_car = gameplay_fabricator->create_ai_car(selected_car_configuration_filename.str());
                        gameplay_fabricator->place_car_on_level(level, main_car, 0);
                        main_car->add_component(std::make_shared<ces_car_camera_follow_component>());
                        main_car->get_component<ces_car_camera_follow_component>()->preview_mode = ces_car_camera_follow_component::e_preview_mode::e_none;
                        gameplay_fabricator->customize_car(main_car, selected_car);
                        root->add_child(main_car);
                        
                        const auto car_parts_component = main_car->get_component<ces_car_parts_component>();
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_name_label)->visible = false;
                        
                        glm::vec3 main_car_rotation = main_car->rotation;
                        get_current_camera_3d()->set_rotation(main_car_rotation.y - 90.f);
                        
                        ui_menus_helper::create_main_menu_ui(root, gameplay_ui_fabricator, true);
                        
                        user_database_component->update_rank_according_stars_count(1);
                    }
                    else if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_garage)
                    {
                        const auto level = gameplay_fabricator->create_scene("garage.tmx");
                        root->add_child(level);
                        
                        std::shared_ptr<ces_garage_database_component::garage_dto::car_dto> selected_car = nullptr;
                        const auto garage_database_component = root->get_component<ces_garage_database_component>();
                        garage_database_component->update_cars_according_rank(1, user_database_component->get_rank(1));
                        i32 current_rank = user_database_component->get_rank(1);
                        i32 claimed_rank = user_database_component->get_claimed_rank(1);
                        if (current_rank != claimed_rank)
                        {
                            selected_car = garage_database_component->get_car(1, current_rank);
                            user_database_component->update_claimed_rank(1);
                        }
                        else
                        {
                            selected_car = garage_database_component->get_selected_car(1);
                        }
                        garage_database_component->set_previewed_car_id(selected_car->get_id());
                        
                        std::stringstream selected_car_configuration_filename;
                        selected_car_configuration_filename<<"car_0";
                        selected_car_configuration_filename<<selected_car->get_id();
                        const auto main_car = gameplay_fabricator->create_ai_car(selected_car_configuration_filename.str());
                        gameplay_fabricator->place_car_on_level(level, main_car, 0);
                        main_car->add_component(std::make_shared<ces_car_camera_follow_component>());
                        main_car->get_component<ces_car_camera_follow_component>()->preview_mode = ces_car_camera_follow_component::e_preview_mode::e_1;
                        gameplay_fabricator->customize_car(main_car, selected_car);
                        const auto car_descriptor_component = main_car->get_component<ces_car_descriptor_component>();
                        const auto car_upgrade = car_descriptor_component->get_car_upgrade();
                        car_upgrade->m_car_body_color_id = selected_car->get_car_body_color_id();
                        car_upgrade->m_car_windshield_color_id = selected_car->get_car_windows_color_id();
                        car_upgrade->m_car_speed_upgrade_value = selected_car->get_car_speed_upgrade();
                        car_upgrade->m_car_handling_upgrade_value = selected_car->get_car_handling_upgrade();
                        car_upgrade->m_car_rigidity_upgrade_value = selected_car->get_car_rigidity_upgrade();
                        car_descriptor_component->get_car_upgrade_cache()->apply(car_upgrade);
                        
                        root->add_child(main_car);
                        
                        const auto car_parts_component = main_car->get_component<ces_car_parts_component>();
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_name_label)->visible = false;
                        
                        glm::vec3 main_car_rotation = main_car->rotation;
                        get_current_camera_3d()->set_rotation(main_car_rotation.y + 45.f);
                        
                        ui_menus_helper::create_garage_menu_ui(root, gameplay_ui_fabricator, true);
                        
                        garage_database_component->update_cars_according_rank(1, user_database_component->get_rank(1));
                    }
                    else if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_in_game)
                    {
                        system_modifiers_component->pause_system(ces_car_simulator_system::class_guid(), false);
                        system_modifiers_component->pause_system(ces_interaction_system::class_guid(), false);
                        system_modifiers_component->pause_system(ces_ai_system::class_guid(), false);
                        system_modifiers_component->pause_system(gb::ces_box2d_system::class_guid(), false);
                        system_modifiers_component->pause_system(ces_car_sound_system::class_guid(), false);
                        
                        i32 playing_level_id = levels_database_component->get_playing_level_id();
                        const auto level_data = levels_database_component->get_level(playing_level_id);
                        const auto level = gameplay_fabricator->create_scene(level_data->get_scene_filename());
                        
                        events_provider::shared_instance()->on_level_enter(level_data->get_id());
                        
                        if (playing_level_id == 1)
                        {
                            const auto level_tutorial_component = std::make_shared<ces_level_tutorial_component>();
                            level_tutorial_component->set_parameters(ces_level_tutorial_component::e_tutorial_id::e_tutorial_id_steer);
                            level->add_component(level_tutorial_component);
                        }
                        root->add_child(level);
                        
                        f32 configuration_complexity = level_data->get_complexity();
                        f32 complexity = glm::mix(configuration_complexity, .1f, static_cast<f32>(glm::min(levels_database_component->get_retries_count(playing_level_id), levels_database_component->get_max_retries_to_simplify_level())) / static_cast<f32>(levels_database_component->get_max_retries_to_simplify_level()));
                        std::cout<<"level started with complexity: "<<complexity<<std::endl;
                        const auto level_descriptor_component = level->get_component<ces_level_descriptor_component>();
                        level_descriptor_component->round_time = level_data->get_session_time_in_seconds();
                        level_descriptor_component->complexity = complexity;
                        level_descriptor_component->start_timestamp = std::get_tick_count();
                        level_descriptor_component->is_started = true;
                        
                        const auto level_route_component = level->get_component<ces_level_route_component>();
                        std::vector<glm::vec2> slow_motion_triggers = level_route_component->slow_motion_triggers;
                        
                        const auto garage_database_component = root->get_component<ces_garage_database_component>();
                        const auto selected_car = garage_database_component->get_selected_car(1);
                        
                        std::stringstream selected_car_configuration_filename;
                        selected_car_configuration_filename<<"car_0";
                        selected_car_configuration_filename<<selected_car->get_id();
                        
                        const auto main_car = gameplay_fabricator->create_player_car(selected_car_configuration_filename.str());
                        gameplay_fabricator->place_car_on_level(level, main_car, 0);
                        main_car->add_component(std::make_shared<ces_car_camera_follow_component>());
                        gameplay_fabricator->customize_car(main_car, selected_car);
                        root->add_child(main_car);
                        
                        const auto car_descriptor_component = main_car->get_component<ces_car_descriptor_component>();
                        car_descriptor_component->max_damage = static_cast<f32>(slow_motion_triggers.size()) * glm::mix(1.f, .3f, complexity);
                        
                        const auto car_parts_component = main_car->get_component<ces_car_parts_component>();
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_label)->visible = false;
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_value_label)->visible = false;
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_label)->visible = false;
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_value_label)->visible = false;
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_label)->visible = false;
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_value_label)->visible = false;
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_direction_arrow)->visible = false;
                        
                        glm::vec3 main_car_rotation = main_car->rotation;
                        get_current_camera_3d()->set_rotation(main_car_rotation.y - 90.f);
                        
                        const auto box2d_body_component = main_car->get_component<gb::ces_box2d_body_component>();
                    
                        i32 ai_car_id_min = glm::clamp(user_database_component->get_rank(1) - 3, 1, 8);
                        i32 ai_car_id_max = glm::clamp(user_database_component->get_rank(1) + 3, 1, 8);
                        ces_car_ai_input_component::e_follow_side follow_side = ces_car_ai_input_component::e_right;
                        for(i32 i = 0; i < level_data->get_ai_cars_count(); ++i)
                        {
                            std::stringstream ai_car_configuration_filename;
                            ai_car_configuration_filename<<"car_0";
                            ai_car_configuration_filename<<std::get_random_i(ai_car_id_min, ai_car_id_max);
                            
                            const auto ai_car = gameplay_fabricator->create_ai_car(ai_car_configuration_filename.str());
                            gameplay_fabricator->place_car_on_level(level, ai_car, 1);
                            // gameplay_fabricator->reskin_car(ai_car, ai_car_configuration_filename.str(), std::get_random_i(1, 3));
                            root->add_child(ai_car);
                            box2d_body_component->add_to_contact_ignore_list(ai_car);
                            
                            const auto car_ai_input_component = ai_car->get_component<ces_car_ai_input_component>();
                            car_ai_input_component->follow_side = follow_side;
                            if (follow_side == ces_car_ai_input_component::e_right)
                            {
                                follow_side = ces_car_ai_input_component::e_left;
                            }
                            else
                            {
                                follow_side = ces_car_ai_input_component::e_right;
                            }
                        }
                        
                        system_modifiers_component->pause_system(ces_car_simulator_system::class_guid(), true);
                        system_modifiers_component->pause_system(ces_interaction_system::class_guid(), true);
                        system_modifiers_component->pause_system(ces_ai_system::class_guid(), true);
                        
                        ui_menus_helper::create_in_game_menu_ui(root, gameplay_ui_fabricator, true);
                    }
                    scene_state_automat_component->state = ces_scene_state_automat_component::e_state_loading;
                }
                scene_state_automat_component->loading_progress = loading_progress;
            }
            else if (scene_state_automat_component->state == ces_scene_state_automat_component::e_state_loading)
            {
                f32 loading_progress = scene_state_automat_component->loading_progress;
                scene_visual_effects_component->is_crossfade_enabled = true;
                scene_visual_effects_component->crossfade_progress = loading_progress;
                scene_visual_effects_component->is_noises_enabled = false;
                
                if (loading_progress >= 0.f)
                {
                    loading_progress -= ces_scene_state_automat_component::k_loading_progress_inc_value;
                    f32 show_progress = 1.f - loading_progress;
                    if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_main_menu)
                    {
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_open_levels_list_dialog_button),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_career_label),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_open_garage_button),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_garage_label),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_open_shop_button),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_shop_label),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_quit_game_button),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_facebook_button),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_goto_racing1_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_goto_racing2_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_tickets_label),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_cash_label),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_stars_progress_bar),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_stars_progress_label),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_stars_progress_info_label),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_up(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_stars_progress_button),
                                                          show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_rank_info_label),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                    }
                    else if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_garage)
                    {
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_back_from_garage_button),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_back_from_garage_label),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_garage_preview_button),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_body_color_1_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_body_color_2_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_body_color_3_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_body_color_4_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_windows_color_1_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_windows_color_2_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_windows_color_3_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_apply_windows_color_4_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_body_paint_label),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_windshield_paint_label),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_performance_label),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_label),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_progress_bar),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_plus_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_label),
                                                           gameplay_ui_fabricator->get_screen_size(),
                                                           show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_progress_bar),
                                                           gameplay_ui_fabricator->get_screen_size(),
                                                           show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_plus_button),
                                                           gameplay_ui_fabricator->get_screen_size(),
                                                           show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_rigidity_label),
                                                           gameplay_ui_fabricator->get_screen_size(),
                                                           show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_rigidity_progress_bar),
                                                           gameplay_ui_fabricator->get_screen_size(),
                                                           show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_upgrade_rigidity_plus_button),
                                                           gameplay_ui_fabricator->get_screen_size(),
                                                           show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_buy_upgrade_button),
                                                           gameplay_ui_fabricator->get_screen_size(),
                                                           show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_buy_upgrade_label),
                                                           gameplay_ui_fabricator->get_screen_size(),
                                                           show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_next_car_in_garage_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_prev_car_in_garage_button),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_car_skin_1_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_car_skin_2_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_car_skin_3_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_left(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_tickets_label),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_cash_label),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_down(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_select_car_button),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                        ui_animation_helper::show_from_down(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_unlock_car_button),
                                                            gameplay_ui_fabricator->get_screen_size(),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_buy_car_button),
                                                            gameplay_ui_fabricator->get_screen_size(),
                                                            show_progress);
                        
                        ui_animation_helper::show_from_right(ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_buy_car_label),
                                                             gameplay_ui_fabricator->get_screen_size(),
                                                             show_progress);
                        
                       
                    }
                    
                }
                else
                {
                    scene_visual_effects_component->is_crossfade_enabled = false;
                    scene_visual_effects_component->is_noises_enabled = true;
                    loading_progress = 0.f;
                    scene_state_automat_component->state = ces_scene_state_automat_component::e_state_none;
                    if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_main_menu)
                    {
                        const auto sound_component = root->get_component<gb::al::ces_sound_component>();
                        sound_component->trigger_sound("music_01.mp3", false);
                    }
                    if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_garage)
                    {
                        const auto sound_component = root->get_component<gb::al::ces_sound_component>();
                        sound_component->trigger_sound("music_05.mp3", false);
                    }
                    else if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_in_game)
                    {
                        const auto sound_component = root->get_component<gb::al::ces_sound_component>();
                        sound_component->trigger_sound("music_03.mp3", false);
                    }
                }
                scene_state_automat_component->loading_progress = loading_progress;
            }
        });
        
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_main_car_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            m_main_car = entity;
            std::string id = entity->tag;
            m_all_cars[id] = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_ai_car_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            std::string id = entity->tag;
            m_all_cars[id] = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_ui_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            auto ui_interaction_component = entity->get_component<ces_ui_interaction_component>();
            ui_controls_helper::add_control(entity);
        });
    }
    
    void ces_state_automat_system::on_feed_end(f32 dt)
    {
        
    }
}
