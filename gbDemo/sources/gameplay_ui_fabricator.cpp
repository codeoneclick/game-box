//
//  gameplay_ui_fabricator.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 8/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "gameplay_ui_fabricator.h"
#include "gameplay_configuration_accessor.h"
#include "scene_fabricator.h"
#include "ui_fabricator.h"
#include "game_object_2d.h"
#include "sprite.h"
#include "ces_transformation_2d_component.h"
#include "ces_ui_interaction_component.h"
#include "ces_bound_touch_2d_component.h"
#include "dialog.h"
#include "button.h"
#include "panel.h"
#include "image_button.h"
#include "joystick.h"
#include "textfield.h"
#include "table_view.h"
#include "action_console.h"
#include "progress_bar.h"
#include "color_picker.h"
#include "ces_car_model_component.h"
#include "ui_controls_helper.h"
#include "ui_animation_helper.h"

namespace game
{
    
#if defined(__OSX__)
    
    const glm::vec2 gameplay_ui_fabricator::k_image_button_size = glm::vec2(96.f);
    
#else
    
    const glm::vec2 gameplay_ui_fabricator::k_image_button_size = glm::vec2(48.f);
    
#endif
    
    const glm::u8vec4 gameplay_ui_fabricator::k_control_image_color  = glm::u8vec4(214, 157, 214, 255);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_text_color = glm::u8vec4(214, 214, 157, 255);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_background_color = glm::u8vec4(26, 47, 52, 127);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_foreground_color = glm::u8vec4(26, 47, 52, 127);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_image_disabled_color = glm::u8vec4(214 - 32, 157 - 32, 214 - 32, 255 - 64);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_text_disabled_color = glm::u8vec4(214 - 32, 214 - 32, 157 - 32, 255 - 64);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_background_disabled_color = glm::u8vec4(26 - 16, 47 - 16, 52 - 16, 127 - 64);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_foreground_disabled_color = glm::u8vec4(26 - 16, 47 - 16, 52 - 16, 127 - 64);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_image_focused_color = glm::u8vec4(214 + 32, 157 + 32, 214 + 32, 255);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_text_focused_color = glm::u8vec4(214 + 32, 214 + 32, 157 + 32, 255);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_background_focused_color = glm::u8vec4(26 + 64, 47 + 64, 52 + 64, 127 + 64);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_foreground_focused_color = glm::u8vec4(26 + 64, 47 + 64, 52 + 64, 127 + 64);
    
    const glm::vec2 gameplay_ui_fabricator::k_label_size = glm::vec2(220.f, 32.f);
    const glm::vec2 gameplay_ui_fabricator::k_progress_bar_size = glm::vec2(220.f, 32.f);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_transparent_color = glm::u8vec4(1);
    
    gameplay_ui_fabricator::gameplay_ui_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator,
                                                   const gb::ui::ui_fabricator_shared_ptr& ui_base_fabricator,
                                                   const glm::ivec2& screen_size) :
    m_general_fabricator(general_fabricator),
    m_ui_base_fabricator(ui_base_fabricator),
    m_screen_size(screen_size)
    {
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_open_levels_list_dialog_button)] = glm::vec2(32.f, 40.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_career_label)] = glm::vec2(32.f + k_image_button_size.x, 40.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_open_garage_button)] = glm::vec2(32.f, 40.f + 8.f + k_image_button_size.y);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_garage_label)] = glm::vec2(32.f + k_image_button_size.x, 40.f + 8.f + k_image_button_size.y);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_open_shop_button)] = glm::vec2(32.f, 40.f + 8.f * 2.f + k_image_button_size.y * 2.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_shop_label)] = glm::vec2(32.f + k_image_button_size.x, 40.f + 8.f * 2.f + k_image_button_size.y * 2.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_leaderboard_button)] = glm::vec2(32.f, 40.f + 8.f * 3.f + k_image_button_size.y * 3.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_leaderboard_label)] = glm::vec2(32.f + k_image_button_size.x, 40.f + 8.f * 3.f + k_image_button_size.y * 3.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_daily_tasks_button)] = glm::vec2(32.f, 40.f + 8.f * 4.f + k_image_button_size.y * 4.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_daily_tasks_label)] = glm::vec2(32.f + k_image_button_size.x, 40.f + 8.f * 4.f + k_image_button_size.y * 4.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_controls_button)] = glm::vec2(32.f, 40.f + 8.f * 5.f + k_image_button_size.y * 5.f);
         ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_controls_label)] = glm::vec2(32.f + k_image_button_size.x, 40.f + 8.f * 5.f + k_image_button_size.y * 5.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_quit_game_button)] = glm::vec2(32.f, 40.f + 8.f * 6.f + k_image_button_size.y * 6.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_facebook_button)] = glm::vec2(32.f, 40.f + 8.f * 3.f + k_image_button_size.y * 3.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_back_from_garage_button)] = glm::vec2(32.f, 40.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_back_from_garage_label)] = glm::vec2(32.f + k_image_button_size.x, 40.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_garage_preview_button)] = glm::vec2(32.f, 40.f + 8.f + k_image_button_size.y);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_zoom_label)] = glm::vec2(32.f + k_image_button_size.x, 40.f + 8.f + k_image_button_size.y);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_next_car_in_garage_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f, m_screen_size.y - k_image_button_size.y - 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_prev_car_in_garage_button)] = glm::vec2(32.f, m_screen_size.y - k_image_button_size.y - 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_skin_1_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f, 40.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_skin_2_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f, 40.f + k_image_button_size.y + 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_skin_3_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_left_label)] = glm::vec2(32.f, m_screen_size.y * .5f + 24.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_right_label)] = glm::vec2(m_screen_size.x - 128.f, m_screen_size.y * .5f + 24.f);
         ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_label)] = glm::vec2(m_screen_size.x * .5f - 200.f, m_screen_size.y * .5f - 300.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_goto_racing1_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f, m_screen_size.y - k_image_button_size.y - 8.f);
         ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_goto_racing2_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x * 2.f - 32.f - k_image_button_size.x, m_screen_size.y - k_image_button_size.y - 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_pause_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f, 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_tickets_label)] = glm::vec2(32.f, 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_cash_label)] = glm::vec2(m_screen_size.x - 32.f - 210.f, 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_select_car_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f - 8.f - k_image_button_size.x, m_screen_size.y - k_image_button_size.y - 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_unlock_car_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f - 8.f - k_image_button_size.x, m_screen_size.y - k_image_button_size.y - 8.f);
         ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_label)] = glm::vec2(m_screen_size.x - 32.f - 210.f, 8.f + 8.f + k_label_size.y);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_bar)] = glm::vec2(m_screen_size.x - 32.f - 210.f, 8.f + 8.f + 8.f + k_label_size.y * 2.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_info_label)] = glm::vec2(m_screen_size.x - 32.f - 105.f, 8.f + 8.f + 8.f + 8.f + k_label_size.y * 3.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_rank_info_label)] = glm::vec2(m_screen_size.x - 32.f - 210.f, 8.f + 8.f + 8.f + 8.f + k_label_size.y * 3.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_button)] = glm::vec2(m_screen_size.x - 32.f - 210.f - 8.f - k_image_button_size.x, 8.f + 8.f + k_label_size.y);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_damage_bar)] = glm::vec2(m_screen_size.x - 32.f - 150.f - 8.f - k_image_button_size.x, 32.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_damage_label)] = glm::vec2(m_screen_size.x - 32.f - 150.f - 8.f - k_image_button_size.x, 8.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_body_paint_label)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - 160.f, 40.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_windshield_paint_label)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - 160.f,  40.f + k_image_button_size.y + 8.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_performance_label)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - 160.f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_buy_upgrade_label)] = glm::vec2(m_screen_size.x - 32.f - k_image_button_size.x * 3.f, m_screen_size.y - (k_image_button_size.y + 8.f) * 3.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_buy_upgrade_button)] = glm::vec2(m_screen_size.x - 32.f - k_image_button_size.x, m_screen_size.y - (k_image_button_size.y + 8.f) * 3.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_label)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - (k_image_button_size.x * .5f + 8.f) * 4.f + 8.f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_progress_bar)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - (k_image_button_size.x * .5f + 8.f) * 4.f + k_image_button_size.x * .5f + 8.f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y * .5f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_plus_button)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - k_image_button_size.y * .5f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y * .5f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_reset_button)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - (k_image_button_size.x * .5f + 8.f) * 4.f + 8.f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y * .5f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_label)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - (k_image_button_size.x * .5f + 8.f) * 4.f + 8.f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_progress_bar)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - (k_image_button_size.x * .5f + 8.f) * 4.f + k_image_button_size.x * .5f + 8.f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y * .5f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_plus_button)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - k_image_button_size.y * .5f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y * .5f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_reset_button)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - (k_image_button_size.x * .5f + 8.f) * 4.f + 8.f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y * .5f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_upgrade_rigidity_label)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - (k_image_button_size.x * .5f + 8.f) * 4.f + 8.f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_upgrade_rigidity_progress_bar)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - (k_image_button_size.x * .5f + 8.f) * 4.f + k_image_button_size.x * .5f + 8.f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y * .5f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_upgrade_rigidity_plus_button)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - k_image_button_size.y * .5f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y * .5f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_upgrade_durability_reset_button)] = glm::vec2(m_screen_size.x - 32.f - 8.f - k_image_button_size.x - (k_image_button_size.x * .5f + 8.f) * 4.f + 8.f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f + k_image_button_size.y * .5f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_buy_car_label)] = glm::vec2(m_screen_size.x - 32.f - k_image_button_size.x * 2.f, m_screen_size.y - (k_image_button_size.y + 8.f) * 2.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_buy_car_button)] = glm::vec2(m_screen_size.x - 32.f - k_image_button_size.x - k_image_button_size.x * 2.f, m_screen_size.y - (k_image_button_size.y + 8.f) * 2.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_career_dialog)] = glm::vec2(40.f + k_image_button_size.x + 16.f, 40.f + 16.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_shop_dialog)] = glm::vec2(40.f + k_image_button_size.x + 16.f, 40.f + 16.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_daily_tasks_dialog)] = glm::vec2(40.f + k_image_button_size.x + 16.f, 40.f + 16.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_body_paint_dialog)] = glm::vec2(m_screen_size.x - 32.f - 16.f - k_image_button_size.x - (k_image_button_size.x * .75f) * 4.f, 40.f + 16.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_windshield_paint_dialog)] = glm::vec2(m_screen_size.x - 32.f - 16.f - k_image_button_size.x - (k_image_button_size.x * .75f) * 4.f, 40.f + 16.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_performance_upgrade_dialog)] = glm::vec2(m_screen_size.x - 32.f - 16.f - k_image_button_size.x - (k_image_button_size.x * .75f) * 4.f, 40.f + 16.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_slow_motion_boost_label)] = glm::vec2(m_screen_size.x - 32.f - 150.f - 100.f - 8.f, 32.f + 8.f + k_image_button_size.y);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_slow_motion_progress_bar)] = glm::vec2(m_screen_size.x - 32.f - 100.f, 32.f + 8.f + k_image_button_size.y);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_speed_up_boost_label)] = glm::vec2(m_screen_size.x - 32.f - 150.f - 100.f - 8.f, 32.f + 8.f + k_image_button_size.y + 24.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_speed_up_boost_progress_bar)] = glm::vec2(m_screen_size.x - 32.f - 100.f, 32.f + 8.f + k_image_button_size.y + 24.f);
    
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_speed_up_boost_trigger_label)] = glm::vec2(m_screen_size.x * .5f - 90.f, m_screen_size.y - (k_image_button_size.y + 8.f) * 2.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_slow_motion_boost_trigger_dialog)] = glm::vec2(m_screen_size.x * .5f - m_screen_size.x * .6f * .5f, m_screen_size.y - (k_image_button_size.y + 8.f) - 32.f);
        
        
        ui_animation_helper::set_screen_size(m_screen_size);
    }
    
    glm::ivec2 gameplay_ui_fabricator::get_screen_size() const
    {
        return m_screen_size;
    }
    
    void gameplay_ui_fabricator::colorize_ui_control(const gb::ui::textfield_shared_ptr& control)
    {
        control->set_text_color(k_control_text_disabled_color, gb::ui::e_control_state::e_disabled);
        control->set_background_color(k_control_background_disabled_color, gb::ui::e_control_state::e_disabled);
        control->set_foreground_color(k_control_foreground_disabled_color, gb::ui::e_control_state::e_disabled);
        
        control->set_text_color(k_control_text_focused_color, gb::ui::e_control_state::e_focused);
        control->set_background_color(k_control_background_focused_color, gb::ui::e_control_state::e_focused);
        control->set_foreground_color(k_control_foreground_focused_color, gb::ui::e_control_state::e_focused);
        
        control->set_text_color(k_control_text_color);
        control->set_background_color(k_control_background_color);
        control->set_foreground_color(k_control_foreground_color);
    }
    
    void gameplay_ui_fabricator::colorize_ui_control(const gb::ui::image_button_shared_ptr& control)
    {
        control->set_image_color(k_control_image_disabled_color, gb::ui::e_control_state::e_disabled);
        control->set_background_color(k_control_background_disabled_color, gb::ui::e_control_state::e_disabled);
        control->set_foreground_color(k_control_foreground_disabled_color, gb::ui::e_control_state::e_disabled);
        
        control->set_image_color(k_control_image_focused_color, gb::ui::e_control_state::e_focused);
        control->set_background_color(k_control_background_focused_color, gb::ui::e_control_state::e_focused);
        control->set_foreground_color(k_control_foreground_focused_color, gb::ui::e_control_state::e_focused);
        
        control->set_image_color(k_control_image_focused_color, gb::ui::e_control_state::e_selected);
        control->set_background_color(k_control_background_focused_color, gb::ui::e_control_state::e_selected);
        control->set_foreground_color(k_control_foreground_focused_color, gb::ui::e_control_state::e_selected);
        
        control->set_image_color(k_control_image_color);
        control->set_background_color(k_control_background_color);
        control->set_foreground_color(k_control_foreground_color);
    }
    
    void gameplay_ui_fabricator::colorize_ui_control(const gb::ui::button_shared_ptr& control)
    {
        control->set_text_color(k_control_text_disabled_color, gb::ui::e_control_state::e_disabled);
        control->set_background_color(k_control_background_disabled_color, gb::ui::e_control_state::e_disabled);
        control->set_foreground_color(k_control_foreground_disabled_color, gb::ui::e_control_state::e_disabled);
        
        control->set_text_color(k_control_text_focused_color, gb::ui::e_control_state::e_focused);
        control->set_background_color(k_control_background_focused_color, gb::ui::e_control_state::e_focused);
        control->set_foreground_color(k_control_foreground_focused_color, gb::ui::e_control_state::e_focused);
        
        control->set_text_color(k_control_text_focused_color, gb::ui::e_control_state::e_selected);
        control->set_background_color(k_control_background_focused_color, gb::ui::e_control_state::e_selected);
        control->set_foreground_color(k_control_foreground_focused_color, gb::ui::e_control_state::e_selected);
        
        control->set_text_color(k_control_text_color);
        control->set_background_color(k_control_background_color);
        control->set_foreground_color(k_control_foreground_color);
    }
    
    void gameplay_ui_fabricator::colorize_ui_control(const gb::ui::dialog_shared_ptr& control)
    {
        control->set_background_color(k_control_background_disabled_color, gb::ui::e_control_state::e_disabled);
        control->set_background_color(k_control_background_focused_color, gb::ui::e_control_state::e_focused);
        control->set_background_color(k_control_background_focused_color, gb::ui::e_control_state::e_selected);
        control->set_background_color(k_control_background_color);
    }
    
    void gameplay_ui_fabricator::add_ui_interaction_component(const gb::ces_entity_shared_ptr& control, ces_ui_interaction_component::e_ui ui)
    {
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(ui);
        control->add_component(ui_interaction_component);
        ui_controls_helper::add_control(control);
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_career_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_trophy.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_open_levels_list_dialog_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_open_levels_list_dialog_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_open_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_gear.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_open_garage_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_open_garage_button)];
        colorize_ui_control(button);
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_open_shop_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_cart.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_open_shop_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_open_shop_button);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_back_from_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_prev.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_back_from_garage_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_back_from_garage_button)];
        colorize_ui_control(button);
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_career_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        colorize_ui_control(dialog);
        dialog->visible = false;
        dialog->show_title(false);
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_career_dialog);
        
        auto table_view = m_ui_base_fabricator.lock()->create_table_view(glm::vec2(256.f,
                                                                                   m_screen_size.y - 56.f * 2.f));
        table_view->position = glm::vec2(0.f);
        table_view->set_background_color(k_control_transparent_color);
        dialog->add_control(table_view, game::ces_ui_interaction_component::k_career_dialog_table_view);
        dialog->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_career_dialog)];
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_countdown_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(220.f, 24.f), "TIME LEFT: 00:00 sec");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_countdown_label);
        label->position = glm::vec2(m_screen_size.x * .5f - 220.f * .5f,
                                    8.f);
        label->set_text_color(k_control_text_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        label->visible = false;
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_next_car_in_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_prev.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_prev_car_in_garage_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_prev_car_in_garage_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_prev_car_in_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_next.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_next_car_in_garage_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_next_car_in_garage_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_skin_1_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_color.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_car_skin_1_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_skin_1_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_skin_2_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_color.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_car_skin_2_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_skin_2_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_skin_3_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_wrench.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_car_skin_3_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_skin_3_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_cars_list_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(100.f));
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_cars_list_dialog);
        dialog->show_title(false);
        
        auto cars_list_table_view = m_ui_base_fabricator.lock()->create_table_view(glm::vec2(130.f,
                                                                                             160.f));
        cars_list_table_view->position = glm::vec2(32.f, 8.f);
        cars_list_table_view->set_background_color(glm::u8vec4(0, 0, 0, 32));
        cars_list_table_view->remove_component(gb::ces_bound_touch_component::class_guid());
        dialog->add_control(cars_list_table_view, game::ces_ui_interaction_component::k_cars_list_dialog_table);
        dialog->visible = true;
        dialog->set_background_color(glm::u8vec4(0));
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_tutorial_steer_left_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(96.f, 192.f), "PRESS");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_left_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_left_label)];
        label->set_text_color(k_control_text_color);
        label->set_visible_edges(false);
        label->set_font_mode(gb::ces_font_component::e_font_mode_edge);
        label->remove_component(gb::ces_bound_touch_component::class_guid());
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_tutorial_steer_right_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(96.f, 192.f), "PRESS");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_right_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_right_label)];
        label->set_text_color(k_control_text_color);
        label->set_visible_edges(false);
        label->set_font_mode(gb::ces_font_component::e_font_mode_edge);
        label->remove_component(gb::ces_bound_touch_component::class_guid());
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_goto_racing1_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_next.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_goto_racing1_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_goto_racing1_button)];
        colorize_ui_control(button);
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_goto_racing2_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_button(glm::vec2(k_image_button_size.x * 2.f, k_image_button_size.y), nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_goto_racing2_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_goto_racing2_button)];
        button->set_text("RACE");
        colorize_ui_control(button);
        button->set_font_size(24.f);
        button->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        button->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_in_game_pause_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_pause.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_pause_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_pause_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_in_game_pause_menu_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(100.f));
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_pause_menu_dialog);
        
        dialog->set_title("PAUSE");
        dialog->set_title_text_color(k_control_text_color);
        
        f32 x_offset = 8.f;
        const auto continue_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_next.png", nullptr);
        continue_button->position = glm::vec2(x_offset, 8.f);
        colorize_ui_control(continue_button);
        continue_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        x_offset += 16.f + k_image_button_size.x;
        
        const auto restart_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_restart.png", nullptr);
        restart_button->position = glm::vec2(x_offset, 8.f);
        colorize_ui_control(restart_button);
        restart_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        x_offset += 16.f + k_image_button_size.x;
        
        const auto exit_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_exit.png", nullptr);
        exit_button->position = glm::vec2(x_offset, 8.f);
        colorize_ui_control(exit_button);
        exit_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        x_offset += 8.f + k_image_button_size.x;
        
        dialog->add_control(continue_button, game::ces_ui_interaction_component::k_pause_menu_dialog_continue_button);
        dialog->add_control(restart_button, game::ces_ui_interaction_component::k_pause_menu_dialog_restart_button);
        dialog->add_control(exit_button, game::ces_ui_interaction_component::k_pause_menu_dialog_exit_button);
        dialog->set_background_color(k_control_background_color);
        dialog->visible = false;
        dialog->position = glm::vec2(m_screen_size.x * .5f - x_offset * .5f, m_screen_size.y * .5 - 48.f);
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_in_game_restart_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_restart_dialog);
        dialog->set_title("RESTART ?");
        dialog->set_title_text_color(k_control_text_color);
        
        std::string message = "";
        
#if defined(__IOS__)
        
        message = "ARE YOU SURE ? IF YOU RESTART THE LEVEL YOU WILL MISS YOUR CURRENT PROGRESS AND ONE TICKET.";
        
#else
        
        message = "ARE YOU SURE ? IF YOU RESTART THE LEVEL YOU WILL MISS YOUR CURRENT PROGRESS.";
        
#endif
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(400.f, 128.f), message);
        
        label->position = glm::vec2(0.f, -24.f);
        label->set_text_color(k_control_text_color);
        label->set_font_name("spincycle.otf");
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        label->set_font_size(20.f);
        label->set_multiline(true);
        
        const auto yes_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        yes_button->position = glm::vec2(400.f * .5f - k_image_button_size.x * 2.f, 80.f);
        yes_button->set_image_color(glm::u8vec4(64, 64, 255, 255));
        yes_button->set_background_color(glm::u8vec4(0, 96, 0, 96));
        yes_button->set_foreground_color(k_control_foreground_color);
        yes_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto no_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_cross.png", nullptr);
        no_button->position = glm::vec2(400.f * .5f + k_image_button_size.x, 80.f);
        no_button->set_image_color(glm::u8vec4(255, 64, 64, 255));
        no_button->set_background_color(glm::u8vec4(96, 0, 0, 96));
        no_button->set_foreground_color(k_control_foreground_color);
        no_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);

        dialog->add_control(label, game::ces_ui_interaction_component::k_restart_dialog_label);
        dialog->add_control(yes_button, game::ces_ui_interaction_component::k_restart_dialog_yes_button);
        dialog->add_control(no_button, game::ces_ui_interaction_component::k_restart_dialog_no_button);
        dialog->visible = false;
        
        glm::vec2 dialog_size = dialog->size;
        dialog->position = glm::vec2(m_screen_size.x * .5f - dialog_size.x * .5f,
                                     m_screen_size.y * .5f - dialog_size.y * .5f);
        dialog->set_background_color(k_control_background_color);
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_in_game_quit_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_quit_dialog);
        dialog->set_title("QUIT ?");
        dialog->set_title_text_color(k_control_text_color);
        
        std::string message = "";
        
#if defined(__IOS__)
        
        message = "ARE YOU SURE ? IF YOU QUIT FROM RACE YOU WILL MISS YOUR CURRENT PROGRESS AND ONE TICKET.";
        
#else
        
        message = "ARE YOU SURE ? IF YOU QUIT FROM RACE YOU WILL MISS YOUR CURRENT PROGRESS.";
        
#endif
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(420.f, 128.f), message);
        
        label->position = glm::vec2(0.f, -24.f);
        label->set_font_name("spincycle.otf");
        label->set_text_color(k_control_text_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        label->set_font_size(20.f);
        label->set_multiline(true);
        
        const auto yes_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        yes_button->position = glm::vec2(420.f * .5f - k_image_button_size.x * 2.f, 80.f);
        yes_button->set_image_color(glm::u8vec4(64, 64, 255, 255));
        yes_button->set_background_color(glm::u8vec4(0, 96, 0, 96));
        yes_button->set_foreground_color(k_control_foreground_color);
        yes_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto no_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_cross.png", nullptr);
        no_button->position = glm::vec2(420.f * .5f + k_image_button_size.x, 80.f);
        no_button->set_image_color(glm::u8vec4(255, 64, 64, 255));
        no_button->set_background_color(glm::u8vec4(96, 0, 0, 96));
        no_button->set_foreground_color(k_control_foreground_color);
        no_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        dialog->add_control(label, game::ces_ui_interaction_component::k_quit_dialog_label);
        dialog->add_control(yes_button, game::ces_ui_interaction_component::k_quit_dialog_yes_button);
        dialog->add_control(no_button, game::ces_ui_interaction_component::k_quit_dialog_no_button);
        dialog->visible = false;
        glm::vec2 dialog_size = dialog->size;
        dialog->position = glm::vec2(m_screen_size.x * .5f - dialog_size.x * .5f,
                                     m_screen_size.y * .5f - dialog_size.y * .5f);
        dialog->set_background_color(k_control_background_color);
        
        return dialog;
    }
    
#if defined(__IOS__)
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_tickets_label(const std::string &filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(210.f, k_label_size.y), "TICKETS: 3");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_tickets_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_tickets_label)];
        label->set_font_size(18.f);
        colorize_ui_control(label);
        
        const auto plus_button =  m_ui_base_fabricator.lock()->create_image_button(glm::vec2(32.f, 32.f), "ui_plus.png", nullptr);
        add_ui_interaction_component(plus_button, game::ces_ui_interaction_component::e_ui::e_ui_tickets_plus_button);
        plus_button->position = glm::vec2(208.f, 0.f);
        colorize_ui_control(plus_button);
        plus_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        label->add_child(plus_button);
        return label;
    }
    
#endif
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_win_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_win_dialog);
        dialog->set_title("WIN");
        dialog->set_title_text_color(k_control_text_color);
        
        const auto place_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(450.f, 24.f), "FINISHED AT PLACE: SECOND");
        
        place_label->position = glm::vec2(0.f, -64.f);
        place_label->set_text_color(k_control_text_color);
        place_label->set_background_color(k_control_background_color);
        place_label->set_foreground_color(k_control_foreground_color);
        place_label->set_visible_edges(false);
        place_label->set_background_color(glm::u8vec4(96, 96, 96, 96));
        
        const auto cash_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(450.f, 24.f), "CASH: 1000");
        
        cash_label->position = glm::vec2(0.f, -32.f);
        cash_label->set_text_color(k_control_text_color);
        cash_label->set_background_color(k_control_background_color);
        cash_label->set_foreground_color(k_control_foreground_color);
        cash_label->set_visible_edges(false);
        cash_label->set_background_color(glm::u8vec4(96, 96, 96, 96));
        
        const auto drift_time_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(450.f, 24.f), "DRIFT TIME: 19:45 sec");
        
        drift_time_label->position = glm::vec2(0.f, 0.f);
        drift_time_label->set_text_color(k_control_text_color);
        drift_time_label->set_background_color(k_control_background_color);
        drift_time_label->set_foreground_color(k_control_foreground_color);
        drift_time_label->set_visible_edges(false);
        drift_time_label->set_background_color(glm::u8vec4(96, 96, 96, 96));
        
        const auto star1_image = m_general_fabricator.lock()->create_sprite("ui_image.xml", "ui_star.png");
        star1_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
        star1_image->size = glm::vec2(64.f, 64.f);
        star1_image->color = glm::u8vec4(192, 0, 192, 255);
        star1_image->position = glm::vec2(450.f * .5f - 75.f - 64.f * .5f, 24.f);
        
        const auto star1_achievement_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(150.f, 24.f), "LOW DAMAGE");
        
        star1_achievement_label->position = glm::vec2(450.f * .5f - 150.f - 75.f, 92.f);
        star1_achievement_label->set_text_color(k_control_text_color);
        star1_achievement_label->set_background_color(k_control_transparent_color);
        star1_achievement_label->set_foreground_color(k_control_transparent_color);
        star1_achievement_label->set_visible_edges(false);
        star1_achievement_label->set_font_size(17.f);
        
        const auto star2_image = m_general_fabricator.lock()->create_sprite("ui_image.xml", "ui_star.png");
        star2_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
        star2_image->size = glm::vec2(64.f, 64.f);
        star2_image->color = glm::u8vec4(192, 0, 192, 255);
        star2_image->position = glm::vec2(450.f * .5f - 64.f * .5f, 24.f);
        
        const auto star2_achievement_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(150.f, 24.f), "FIRST PLACE");
        
        star2_achievement_label->position = glm::vec2(450.f * .5f - 150.f * .5f, 92.f);
        star2_achievement_label->set_text_color(k_control_text_color);
        star2_achievement_label->set_background_color(k_control_transparent_color);
        star2_achievement_label->set_foreground_color(k_control_transparent_color);
        star2_achievement_label->set_visible_edges(false);
        star2_achievement_label->set_font_size(17.f);
        
        const auto star3_image = m_general_fabricator.lock()->create_sprite("ui_image.xml", "ui_star.png");
        star3_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
        star3_image->size = glm::vec2(64.f, 64.f);
        star3_image->color = glm::u8vec4(32, 32, 32, 255);
        star3_image->position = glm::vec2(450.f * .5f + 75.f * .5, 24.f);
        
        const auto star3_achievement_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(150.f, 24.f), "GOOD DRIFT");
        
        star3_achievement_label->position = glm::vec2(450.f * .5f + 150.f * .5f, 92.f);
        star3_achievement_label->set_text_color(k_control_text_color);
        star3_achievement_label->set_background_color(k_control_transparent_color);
        star3_achievement_label->set_foreground_color(k_control_transparent_color);
        star3_achievement_label->set_visible_edges(false);
        star3_achievement_label->set_font_size(17.f);
        
        const auto restart_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_restart.png", nullptr);
        restart_button->position = glm::vec2(450.f * .5f - k_image_button_size.x * 2.f, 124.f);
        colorize_ui_control(restart_button);
        restart_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto twice_cash_button = m_ui_base_fabricator.lock()->create_button(glm::vec2(k_image_button_size.x * 2.f, k_image_button_size.y), nullptr);
        twice_cash_button->set_text("2X CASH");
        twice_cash_button->position = glm::vec2(450.f * .5f - k_image_button_size.x, 124.f);
        twice_cash_button->set_font_size(17.f);
        twice_cash_button->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        twice_cash_button->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        colorize_ui_control(twice_cash_button);
        twice_cash_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto continue_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_next.png", nullptr);
        continue_button->position = glm::vec2(450.f * .5f + k_image_button_size.x, 124.f);
        colorize_ui_control(continue_button);
        continue_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        dialog->add_control(place_label, game::ces_ui_interaction_component::k_end_game_dialog_place_label);
        dialog->add_control(drift_time_label, game::ces_ui_interaction_component::k_end_game_dialog_drift_time_label);
        dialog->add_control(cash_label, game::ces_ui_interaction_component::k_win_dialog_cash_label);
        dialog->add_control(star1_image, game::ces_ui_interaction_component::k_end_game_dialog_star1_image);
        dialog->add_control(star2_image, game::ces_ui_interaction_component::k_end_game_dialog_star2_image);
        dialog->add_control(star3_image, game::ces_ui_interaction_component::k_end_game_dialog_star3_image);
        dialog->add_control(star1_achievement_label, game::ces_ui_interaction_component::k_win_dialog_star1_achievement_label);
        dialog->add_control(star2_achievement_label, game::ces_ui_interaction_component::k_win_dialog_star2_achievement_label);
        dialog->add_control(star3_achievement_label, game::ces_ui_interaction_component::k_win_dialog_star3_achievement_label);
        dialog->add_control(continue_button, game::ces_ui_interaction_component::k_end_game_dialog_continue_button);
        dialog->add_control(twice_cash_button, game::ces_ui_interaction_component::k_win_dialog_twice_cash_button_button);
        dialog->add_control(restart_button, game::ces_ui_interaction_component::k_end_game_dialog_restart_button);
        
        dialog->visible = false;
        dialog->position = glm::vec2(m_screen_size.x * .5f - 450.f * .5f, m_screen_size.y * .5 - 48.f);
        dialog->set_background_color(k_control_background_color);
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_loose_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_loose_dialog);
        dialog->set_title("LOOSE");
        dialog->set_title_text_color(k_control_text_color);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(420.f, 128.f), "YOUR CAR IS CRASHED");
        
        label->position = glm::vec2(0.f, -30.f);
        label->set_text_color(k_control_text_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        label->set_visible_edges(false);
        
        const auto restart_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_restart.png", nullptr);
        restart_button->position = glm::vec2(420.f * .5f - k_image_button_size.x * 2.f, 80.f);
        colorize_ui_control(restart_button);
        restart_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto continue_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_next.png", nullptr);
        continue_button->position = glm::vec2(420.f * .5f + k_image_button_size.x, 80.f);
        colorize_ui_control(continue_button);
        continue_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        dialog->add_control(label, game::ces_ui_interaction_component::k_loose_dialog_title_label);
        dialog->add_control(restart_button, game::ces_ui_interaction_component::k_loose_dialog_restart_button);
        dialog->add_control(continue_button, game::ces_ui_interaction_component::k_loose_dialog_continue_button);

        dialog->visible = false;
        glm::vec2 dialog_size = dialog->size;
        dialog->position = glm::vec2(m_screen_size.x * .5f - dialog_size.x * .5f,
                                     m_screen_size.y * .5f);
        dialog->set_background_color(k_control_background_color);
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_select_car_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_select_car_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_select_car_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_unlock_car_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_locked.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_unlock_car_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_unlock_car_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_buy_car_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_cart.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_buy_car_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_buy_car_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_buy_car_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(k_image_button_size.x * 2.f, k_image_button_size.y), "10000 $");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_buy_car_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_buy_car_label)];
        colorize_ui_control(label);
        label->set_font_size(16.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_stars_progress_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(210.f, k_label_size.y), "STAR PROGRESS");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_stars_progress_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_label)];
        colorize_ui_control(label);
        label->set_font_name("spincycle.otf");
        label->set_font_size(18.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_stars_progress_info_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(105.f, k_label_size.y), "0/3:");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_stars_progress_info_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_info_label)];
        colorize_ui_control(label);
        label->set_font_name("spincycle.otf");
        label->set_font_size(18.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_stars_progress_bar(const std::string& filename)
    {
        const auto progress_bar = m_ui_base_fabricator.lock()->create_progress_bar(glm::vec2(210.f, k_progress_bar_size.y));
        add_ui_interaction_component(progress_bar, game::ces_ui_interaction_component::e_ui::e_ui_stars_progress_bar);
        progress_bar->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_bar)];
        progress_bar->set_progress_line_color(k_control_text_color);
        progress_bar->set_background_color(k_control_background_color);
        return progress_bar;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_stars_progress_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_star.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_stars_progress_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_rank_info_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(105.f, k_label_size.y), "RANK: 1");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_rank_info_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_rank_info_label)];
        colorize_ui_control(label);
        label->set_font_name("spincycle.otf");
        label->set_font_size(18.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_damage_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(150.f, 24.f), "DAMAGE:");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_car_damage_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_damage_label)];
        label->set_font_name("spincycle.otf");
        colorize_ui_control(label);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_damage_bar(const std::string& filename)
    {
        const auto progress_bar = m_ui_base_fabricator.lock()->create_progress_bar(glm::vec2(150.f, 24.f));
        add_ui_interaction_component(progress_bar, game::ces_ui_interaction_component::e_ui::e_ui_car_damage_bar);
        progress_bar->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_damage_bar)];
        progress_bar->set_progress_line_color(k_control_text_color);
        progress_bar->set_background_color(k_control_background_color);
        return progress_bar;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_full_tickets_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(100.f));
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_full_tickets_dialog);
        
        dialog->set_title("TICKETS");
        dialog->set_title_text_color(k_control_text_color);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(400.f, 24.f), "YOU ALREADY HAVE 5 TICKETS!");
        
        label->set_font_name("spincycle.otf");
        label->position = glm::vec2(0.f, -50.f);
        label->set_text_color(k_control_text_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        
        const auto ok_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        ok_button->position = glm::vec2(400.f * .5f - 24.f, -20.f);
        colorize_ui_control(ok_button);
        ok_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        dialog->position = glm::vec2(m_screen_size.x * .5f - 400.f * .5f, m_screen_size.y * .5 - 48.f);
        dialog->add_control(label, game::ces_ui_interaction_component::k_full_tickets_dialog_title);
        dialog->add_control(ok_button, game::ces_ui_interaction_component::k_full_tickets_dialog_ok_button);
        dialog->visible = false;
        dialog->set_background_color(k_control_background_color);
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_not_enough_tickets_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(100.f));
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_not_enough_tickets_dialog);

        dialog->set_title("TICKETS");
        dialog->set_title_text_color(k_control_text_color);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(400.f, 24.f), "NOT ENOUGH TICKETS TO START RACE!");
        
        label->position = glm::vec2(0.f, -50.f);
        colorize_ui_control(label);
    
        
        const auto ok_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        ok_button->position = glm::vec2(400.f * .5f - 24.f - 32.f, -20.f);
        colorize_ui_control(ok_button);
        ok_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto plus_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_video.png", nullptr);
        plus_button->position = glm::vec2(400.f * .5f - 24.f + 32.f, -20.f);
        colorize_ui_control(plus_button);
        plus_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);

        dialog->add_control(label, game::ces_ui_interaction_component::k_not_enough_tickets_dialog_label);
        dialog->add_control(ok_button, game::ces_ui_interaction_component::k_not_enough_tickets_dialog_ok_button);
        dialog->add_control(plus_button, game::ces_ui_interaction_component::k_not_enough_tickets_dialog_plus_button);
        dialog->visible = false;
        dialog->position = glm::vec2(m_screen_size.x * .5f - 400.f * .5f, m_screen_size.y * .5 - 48.f);
        dialog->set_background_color(k_control_background_color);
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_shop_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        dialog->visible = false;
        dialog->show_title(false);
        colorize_ui_control(dialog);
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_shop_dialog);
        
        auto table_view = m_ui_base_fabricator.lock()->create_table_view(glm::vec2(256.f,
                                                                                   m_screen_size.y - 56.f * 2.f));
        table_view->position = glm::vec2(0.f);
        table_view->set_background_color(k_control_transparent_color);
        dialog->add_control(table_view, game::ces_ui_interaction_component::k_shop_dialog_table_view);
        dialog->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_shop_dialog)];
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_cash_shop_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(100.f));
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_cash_shop_dialog);
        
        dialog->set_title("");
        dialog->set_title_text_color(k_control_text_color);
        dialog->show_title(false);
        
        f32 table_view_height_coefficient = 0.f;
        f32 table_view_offset_from_top = 0.f;
        
#if defined(__IOS__)
        
        table_view_height_coefficient = 2.3f;
        table_view_offset_from_top = 24.f;
        
#else
        
        table_view_height_coefficient = 3.2f;
        table_view_offset_from_top = 128.f;
        
#endif
        
        auto table_view = m_ui_base_fabricator.lock()->create_table_view(glm::vec2(256.f,
                                                                                   table_view_height_coefficient * (96.f + 8.f)));
        table_view->position = glm::vec2(get_screen_size().x * .5f - 128.f - 8.f, table_view_offset_from_top);
        table_view->set_background_color(glm::u8vec4(0, 0, 0, 32));
        
        const auto ok_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_cross.png", nullptr);
        ok_button->position = glm::vec2(get_screen_size().x * .5f - k_image_button_size.x * .5f + 8.f, table_view_height_coefficient * (96.f + 8.f) + table_view_offset_from_top + 8.f);
        colorize_ui_control(ok_button);
        ok_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        dialog->add_control(table_view, game::ces_ui_interaction_component::k_cash_shop_dialog_table_view);
        dialog->add_control(ok_button, game::ces_ui_interaction_component::k_cash_shop_ok_button);
        dialog->visible = false;
        dialog->set_background_color(k_control_background_color);
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_screen_overlay(const std::string& filename)
    {
        const auto overlay = m_general_fabricator.lock()->create_sprite("button_background.xml");
        add_ui_interaction_component(overlay, game::ces_ui_interaction_component::e_ui::e_ui_screen_overlay);
        overlay->color = glm::u8vec4(0, 0, 0, 224);
        overlay->position = glm::vec2(-m_screen_size.x * .5f, -m_screen_size.y * .5f);
        overlay->size = glm::vec2(m_screen_size.x, m_screen_size.y);
        overlay->visible = false;
        return overlay;
    }
    
#if defined(__FACEBOOK_LOGIN__)
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_facebook_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_facebook.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_facebook_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_facebook_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
#endif
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_unlock_car_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(100.f));
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_unlock_car_dialog);
       
        dialog->set_title("UNLOCK");
        dialog->set_title_text_color(k_control_text_color);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(400.f, 24.f), "CAR WILL BE UNLOCKED AT RANK 0");
        
        label->position = glm::vec2(0.f, 0.f);
        label->set_text_color(k_control_text_color);
        label->set_font_name("spincycle.otf");
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        
        const auto ok_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        ok_button->position = glm::vec2(400.f * .5f - k_image_button_size.x * .5f, 50.f);
        colorize_ui_control(ok_button);
        ok_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        dialog->position = glm::vec2(m_screen_size.x * .5f - 400.f * .5f, m_screen_size.y * .5);
        dialog->add_control(label, game::ces_ui_interaction_component::k_car_unlock_dialog_label);
        dialog->add_control(ok_button, game::ces_ui_interaction_component::k_car_unlock_dialog_ok_button);
        dialog->visible = false;
        dialog->set_background_color(k_control_background_color);
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_tutorial_steer_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(400.f, 120.f), "PRESS IN FRONT OF THE CAR AND MOVE IN THE DIRECTION OF THE TURN");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_label)];
        label->set_text_color(k_control_text_color);
        label->set_visible_edges(false);
        label->set_font_mode(gb::ces_font_component::e_font_mode_edge);
        label->remove_component(gb::ces_bound_touch_component::class_guid());
        return label;
    }
    
#if defined(__OSX__)
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_exit_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_exit.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_quit_game_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_quit_game_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }

    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_controls_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_joystick.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_controls_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_controls_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }

    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_controls_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "CONTROLS");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_controls_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_controls_label)];
               colorize_ui_control(label);
        label->set_font_size(24.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_exit_game_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_quit_game_dialog);
        
        dialog->set_title("QUIT ?");
        dialog->set_title_text_color(k_control_text_color);
        
        std::string message = "ARE YOU SURE ?";
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(420.f, 128.f), message);
        label->set_font_name("spincycle.otf");
        label->position = glm::vec2(0.f, -24.f);
        label->set_text_color(k_control_text_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        
        const auto yes_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        yes_button->position = glm::vec2(420.f * .5f - k_image_button_size.x * 2.f, 80.f);
        yes_button->set_image_color(glm::u8vec4(64, 64, 255, 255));
        yes_button->set_background_color(glm::u8vec4(0, 96, 0, 96));
        yes_button->set_foreground_color(k_control_foreground_color);
        yes_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto no_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_cross.png", nullptr);
        no_button->position = glm::vec2(420.f * .5f + k_image_button_size.x, 80.f);
        no_button->set_image_color(glm::u8vec4(255, 64, 64, 255));
        no_button->set_background_color(glm::u8vec4(96, 0, 0, 96));
        no_button->set_foreground_color(k_control_foreground_color);
        no_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        dialog->add_control(label, game::ces_ui_interaction_component::k_quit_dialog_label);
        dialog->add_control(yes_button, game::ces_ui_interaction_component::k_quit_dialog_yes_button);
        dialog->add_control(no_button, game::ces_ui_interaction_component::k_quit_dialog_no_button);
        dialog->visible = false;
        glm::vec2 dialog_size = dialog->size;
        dialog->position = glm::vec2(m_screen_size.x * .5f - dialog_size.x * .5f,
                                     m_screen_size.y * .5f - dialog_size.y * .5f);
        dialog->set_background_color(k_control_background_color);
        
        return dialog;
    }
    
#endif
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_career_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "CAREER");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_career_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_career_label)];
        colorize_ui_control(label);
        label->set_font_size(24.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_garage_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "GARAGE");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_garage_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_garage_label)];
        colorize_ui_control(label);
        label->set_font_size(24.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_back_from_garage_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "BACK");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_back_from_garage_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_back_from_garage_label)];
        colorize_ui_control(label);
        label->set_font_size(24.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_zoom_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "ZOOM");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_zoom_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_zoom_label)];
        colorize_ui_control(label);
        label->set_font_size(24.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_shop_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "SHOP");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_shop_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_shop_label)];
        colorize_ui_control(label);
        label->set_font_size(24.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_garage_preview_mode_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_search.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_garage_preview_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_garage_preview_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_cash_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(210.f, k_label_size.y), "CASH: 100");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_cash_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_cash_label)];
        colorize_ui_control(label);
        label->set_font_name("spincycle.otf");
        label->set_font_size(18.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        
        const auto plus_button =  m_ui_base_fabricator.lock()->create_image_button(glm::vec2(32.f, 32.f), "ui_plus.png", nullptr);
        add_ui_interaction_component(plus_button, game::ces_ui_interaction_component::e_ui::e_ui_cash_plus_button);
        plus_button->position = glm::vec2(178.f, 0.f);
        colorize_ui_control(plus_button);
        plus_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        label->add_child(plus_button);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_body_paint_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "BODY");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_body_paint_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_body_paint_label)];
        colorize_ui_control(label);
        label->set_font_size(18.f);
        label->set_font_name("spincycle.otf");
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_windshield_paint_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "WINDSHIELD");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_windshield_paint_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_windshield_paint_label)];
        colorize_ui_control(label);
        label->set_font_size(18.f);
        label->set_font_name("spincycle.otf");
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_performance_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "PERFORMANCE");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_performance_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_performance_label)];
        colorize_ui_control(label);
        label->set_font_size(18.f);
        label->set_font_name("spincycle.otf");
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_buy_upgrade_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_cart.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_buy_upgrade_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_buy_upgrade_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_buy_upgrade_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(k_image_button_size.x * 2.f, k_image_button_size.y), "10000 $");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_buy_upgrade_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_buy_upgrade_label)];
        colorize_ui_control(label);
        label->set_font_size(16.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_leaderboard_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_leaderboard.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_leaderboard_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_leaderboard_button)];
        colorize_ui_control(button);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_leaderboard_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "DRIFTERS");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_leaderboard_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_leaderboard_label)];
        colorize_ui_control(label);
        label->set_font_size(24.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_body_paint_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        colorize_ui_control(dialog);
        dialog->visible = false;
        dialog->show_title(false);
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_body_paint_dialog);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2((k_image_button_size.x * .75f) * 4.f, k_image_button_size.y), "BODY PAINT");
        label->position = glm::vec2(0.f);
        colorize_ui_control(label);
        label->set_font_size(22.f);
        label->set_font_name("spincycle.otf");
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        
        const auto body_paint_color_picker = m_ui_base_fabricator.lock()->create_color_picker(glm::vec2(144.f));
        body_paint_color_picker->position = glm::vec2(0.f, k_image_button_size.y + 8.f);
        add_ui_interaction_component(body_paint_color_picker, game::ces_ui_interaction_component::e_ui::e_ui_body_paint_color_picker);
        body_paint_color_picker->set_background_color(k_control_transparent_color);
        
        dialog->add_control(label, game::ces_ui_interaction_component::k_body_paint_label);
        dialog->add_control(body_paint_color_picker, game::ces_ui_interaction_component::k_body_paint_color_picker);
        dialog->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_body_paint_dialog)];
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_windshield_paint_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        colorize_ui_control(dialog);
        dialog->visible = false;
        dialog->show_title(false);
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_windshield_paint_dialog);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2((k_image_button_size.x * .75f) * 4.f, k_image_button_size.y), "WINDSHIELD");
        label->position = glm::vec2(0.f);
        colorize_ui_control(label);
        label->set_font_size(22.f);
        label->set_font_name("spincycle.otf");
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        
        const auto color_picker = m_ui_base_fabricator.lock()->create_color_picker(glm::vec2(144.f));
        color_picker->position = glm::vec2(0.f, k_image_button_size.y + 8.f);
        add_ui_interaction_component(color_picker, game::ces_ui_interaction_component::e_ui::e_ui_windshield_paint_color_picker);
        color_picker->set_background_color(k_control_transparent_color);
        
        dialog->add_control(label, game::ces_ui_interaction_component::k_windshield_paint_label);
        dialog->add_control(color_picker, game::ces_ui_interaction_component::k_windshield_paint_color_picker);
        dialog->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_windshield_paint_dialog)];
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_performance_upgrade_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        colorize_ui_control(dialog);
        dialog->visible = false;
        dialog->show_title(false);
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_performance_upgrade_dialog);
        
        const auto speed_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2((k_image_button_size.x * .75f) * 4.f, k_image_button_size.y * .75f), "SPEED");
        speed_label->position = glm::vec2(0.f);
        colorize_ui_control(speed_label);
        speed_label->set_font_size(22.f);
        speed_label->set_font_name("spincycle.otf");
        speed_label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        speed_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        
        const auto handling_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2((k_image_button_size.x * .75f) * 4.f, k_image_button_size.y * .75f), "HADNLING");
        handling_label->position = glm::vec2(0.f, (k_image_button_size.x * .75f) * 2.f);
        colorize_ui_control(handling_label);
        handling_label->set_font_size(22.f);
        handling_label->set_font_name("spincycle.otf");
        handling_label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        handling_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        
        const auto durability_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2((k_image_button_size.x * .75f) * 4.f, k_image_button_size.y * .75f), "DURABILITY");
        durability_label->position = glm::vec2(0.f, (k_image_button_size.x * .75f) * 4.f);
        colorize_ui_control(durability_label);
        durability_label->set_font_size(22.f);
        durability_label->set_font_name("spincycle.otf");
        durability_label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        durability_label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        
        const auto speed_progress_bar = m_ui_base_fabricator.lock()->create_progress_bar(glm::vec2((k_image_button_size.x * .75f) * 2.f, k_image_button_size.y * .75f));
        add_ui_interaction_component(speed_progress_bar, game::ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_progress_bar);
        speed_progress_bar->position = glm::vec2(k_image_button_size.x * .75f, (k_image_button_size.y * .75f));
        speed_progress_bar->set_progress_line_color(k_control_text_color);
        speed_progress_bar->set_background_color(k_control_background_color);
        
        const auto speed_reset_button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(k_image_button_size.y * .75f), "ui_minus.png", nullptr);
        add_ui_interaction_component(speed_reset_button, game::ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_reset_button);
        speed_reset_button->position = glm::vec2(0.f, k_image_button_size.x * .75f);
        colorize_ui_control(speed_reset_button);
        speed_reset_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto speed_plus_button =  m_ui_base_fabricator.lock()->create_image_button(glm::vec2(k_image_button_size.y * .75f), "ui_plus.png", nullptr);
        add_ui_interaction_component(speed_plus_button, game::ces_ui_interaction_component::e_ui::e_ui_upgrade_speed_plus_button);
        speed_plus_button->position = glm::vec2((k_image_button_size.x * .75f) * 3.f, k_image_button_size.x * .75f);
        colorize_ui_control(speed_plus_button);
        speed_plus_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto handling_progress_bar = m_ui_base_fabricator.lock()->create_progress_bar(glm::vec2((k_image_button_size.x * .75f) * 2.f, k_image_button_size.y * .75f));
        add_ui_interaction_component(handling_progress_bar, game::ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_progress_bar);
        handling_progress_bar->position = glm::vec2(k_image_button_size.x * .75f, (k_image_button_size.y * .75f) * 3.f);
        handling_progress_bar->set_progress_line_color(k_control_text_color);
        handling_progress_bar->set_background_color(k_control_background_color);
        
        const auto handling_reset_button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(k_image_button_size.y * .75f), "ui_minus.png", nullptr);
        add_ui_interaction_component(handling_reset_button, game::ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_reset_button);
        handling_reset_button->position = glm::vec2(0.f, (k_image_button_size.x * .75f) * 3.f);
        colorize_ui_control(handling_reset_button);
        handling_reset_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto handling_plus_button =  m_ui_base_fabricator.lock()->create_image_button(glm::vec2(k_image_button_size.y * .75f), "ui_plus.png", nullptr);
        add_ui_interaction_component(handling_plus_button, game::ces_ui_interaction_component::e_ui::e_ui_upgrade_handling_plus_button);
        handling_plus_button->position = glm::vec2((k_image_button_size.x * .75f) * 3.f, (k_image_button_size.x * .75f) * 3.f);
        colorize_ui_control(handling_plus_button);
        handling_plus_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto durabitity_progress_bar = m_ui_base_fabricator.lock()->create_progress_bar(glm::vec2((k_image_button_size.x * .75f) * 2.f, k_image_button_size.y * .75f));
        add_ui_interaction_component(durabitity_progress_bar, game::ces_ui_interaction_component::e_ui::e_ui_upgrade_rigidity_progress_bar);
        durabitity_progress_bar->position = glm::vec2(k_image_button_size.x * .75f, (k_image_button_size.y * .75f) * 5.f);
        durabitity_progress_bar->set_progress_line_color(k_control_text_color);
        durabitity_progress_bar->set_background_color(k_control_background_color);
        
        const auto durabitity_reset_button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(k_image_button_size.y * .75f), "ui_minus.png", nullptr);
        add_ui_interaction_component(durabitity_reset_button, game::ces_ui_interaction_component::e_ui::e_ui_upgrade_durability_reset_button);
        durabitity_reset_button->position = glm::vec2(0.f, (k_image_button_size.x * .75f) * 5.f);
        colorize_ui_control(durabitity_reset_button);
        durabitity_reset_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto durabitity_plus_button =  m_ui_base_fabricator.lock()->create_image_button(glm::vec2(k_image_button_size.y * .75f), "ui_plus.png", nullptr);
        add_ui_interaction_component(durabitity_plus_button, game::ces_ui_interaction_component::e_ui::e_ui_upgrade_rigidity_plus_button);
        durabitity_plus_button->position = glm::vec2((k_image_button_size.x * .75f) * 3.f, (k_image_button_size.x * .75f) * 5.f);
        colorize_ui_control(durabitity_plus_button);
        durabitity_plus_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        dialog->add_control(speed_label, game::ces_ui_interaction_component::k_speed_upgrade_label);
        dialog->add_control(handling_label, game::ces_ui_interaction_component::k_handling_upgrade_label);
        dialog->add_control(durability_label, game::ces_ui_interaction_component::k_durability_upgrade_label);
        dialog->add_control(speed_progress_bar, game::ces_ui_interaction_component::k_speed_upgrade_progress_bar);
        dialog->add_control(speed_reset_button, game::ces_ui_interaction_component::k_speed_upgrade_reset_button);
        dialog->add_control(speed_plus_button, game::ces_ui_interaction_component::k_speed_upgrade_plus_button);
        dialog->add_control(handling_progress_bar, game::ces_ui_interaction_component::k_handling_upgrade_progress_bar);
        dialog->add_control(handling_reset_button, game::ces_ui_interaction_component::k_handling_upgrade_reset_button);
        dialog->add_control(handling_plus_button, game::ces_ui_interaction_component::k_handling_upgrade_plus_button);
        dialog->add_control(durabitity_progress_bar, game::ces_ui_interaction_component::k_durability_upgrade_progress_bar);
        dialog->add_control(durabitity_reset_button, game::ces_ui_interaction_component::k_durability_upgrade_reset_button);
        dialog->add_control(durabitity_plus_button, game::ces_ui_interaction_component::k_durability_upgrade_plus_button);
        dialog->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_performance_upgrade_dialog)];
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_slow_motion_boost_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(150.f, 24.f), "SLOW MOTION");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_slow_motion_boost_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_slow_motion_boost_label)];
        label->set_font_name("spincycle.otf");
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment::e_element_horizontal_aligment_right);
        
        label->set_text_color(k_control_text_disabled_color, gb::ui::e_control_state::e_disabled);
        label->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        label->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        
        label->set_text_color(k_control_text_focused_color, gb::ui::e_control_state::e_focused);
        label->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        label->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        
        label->set_text_color(k_control_text_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_slow_motion_boost_progress_bar(const std::string& filename)
    {
        const auto progress_bar = m_ui_base_fabricator.lock()->create_progress_bar(glm::vec2(100.f, 24.f));
        add_ui_interaction_component(progress_bar, game::ces_ui_interaction_component::e_ui::e_ui_slow_motion_progress_bar);
        progress_bar->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_slow_motion_progress_bar)];
        progress_bar->set_progress_line_color(k_control_text_color);
        progress_bar->set_background_color(k_control_background_color);
        return progress_bar;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_speed_up_boost_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(150.f, 24.f), "SPEED UP");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_speed_up_boost_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_speed_up_boost_label)];
        label->set_font_name("spincycle.otf");
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment::e_element_horizontal_aligment_right);
        
        label->set_text_color(k_control_text_disabled_color, gb::ui::e_control_state::e_disabled);
        label->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        label->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        
        label->set_text_color(k_control_text_focused_color, gb::ui::e_control_state::e_focused);
        label->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        label->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        
        label->set_text_color(k_control_text_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_speed_up_boost_trigger_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(180.f, 48.f), "SPEED UP");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_speed_up_boost_trigger_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_speed_up_boost_trigger_label)];
        label->set_font_name("spincycle.otf");
        label->set_font_mode(gb::ces_font_component::e_font_mode::e_font_mode_edge);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment::e_element_horizontal_aligment_center);
        
        label->set_text_color(k_control_text_disabled_color, gb::ui::e_control_state::e_disabled);
        label->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        label->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        
        label->set_text_color(k_control_text_focused_color, gb::ui::e_control_state::e_focused);
        label->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        label->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        
        label->set_text_color(k_control_text_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_speed_up_boost_progress_bar(const std::string& filename)
    {
        const auto progress_bar = m_ui_base_fabricator.lock()->create_progress_bar(glm::vec2(100.f, 24.f));
        add_ui_interaction_component(progress_bar, game::ces_ui_interaction_component::e_ui::e_ui_speed_up_boost_progress_bar);
        progress_bar->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_speed_up_boost_progress_bar)];
        progress_bar->set_progress_line_color(k_control_text_color);
        progress_bar->set_background_color(k_control_background_color);
        return progress_bar;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_slow_motion_boost_trigger_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        colorize_ui_control(dialog);
        dialog->visible = false;
        dialog->show_title(false);
        dialog->set_background_color(k_control_transparent_color);
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_slow_motion_boost_trigger_dialog);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(m_screen_size.x * .6f, 32.f), "SLOW, SWIPE DOWN");
        label->position = glm::vec2(0.f, 0.f);
        label->set_font_name("spincycle.otf");
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment::e_element_horizontal_aligment_center);
        
        label->set_text_color(k_control_text_disabled_color, gb::ui::e_control_state::e_disabled);
        label->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        label->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        
        label->set_text_color(k_control_text_focused_color, gb::ui::e_control_state::e_focused);
        label->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        label->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        
        label->set_text_color(k_control_text_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        label->remove_component(gb::ces_bound_touch_component::class_guid());
        
        const auto indicator_1 = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size * .66f, "ui_down_hint.png", nullptr);
        indicator_1->position = glm::vec2(0.f, 0.f);
        
        indicator_1->set_image_color(k_control_image_disabled_color, gb::ui::e_control_state::e_disabled);
        indicator_1->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        indicator_1->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        
        indicator_1->set_image_color(k_control_image_focused_color, gb::ui::e_control_state::e_focused);
        indicator_1->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        indicator_1->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        
        indicator_1->set_image_color(k_control_image_color);
        indicator_1->set_background_color(k_control_transparent_color);
        indicator_1->set_foreground_color(k_control_transparent_color);
        indicator_1->remove_component(gb::ces_bound_touch_component::class_guid());
        
        const auto indicator_2 = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size  * .66f, "ui_down_hint.png", nullptr);
        indicator_2->position = glm::vec2(m_screen_size.x * .6f * .5f - k_image_button_size.x  * .66f * .5f, 0.f);
        
        indicator_2->set_image_color(k_control_image_disabled_color, gb::ui::e_control_state::e_disabled);
        indicator_2->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        indicator_2->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        
        indicator_2->set_image_color(k_control_image_focused_color, gb::ui::e_control_state::e_focused);
        indicator_2->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        indicator_2->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        
        indicator_2->set_image_color(k_control_image_color);
        indicator_2->set_background_color(k_control_transparent_color);
        indicator_2->set_foreground_color(k_control_transparent_color);
        indicator_2->remove_component(gb::ces_bound_touch_component::class_guid());
        
        const auto indicator_3 = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size * .66f, "ui_down_hint.png", nullptr);
        indicator_3->position = glm::vec2(m_screen_size.x * .6f - k_image_button_size.x  * .66f, 0.f);
        
        indicator_3->set_image_color(k_control_image_disabled_color, gb::ui::e_control_state::e_disabled);
        indicator_3->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        indicator_3->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_disabled);
        
        indicator_3->set_image_color(k_control_image_focused_color, gb::ui::e_control_state::e_focused);
        indicator_3->set_background_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        indicator_3->set_foreground_color(k_control_transparent_color, gb::ui::e_control_state::e_focused);
        
        indicator_3->set_image_color(k_control_image_color);
        indicator_3->set_background_color(k_control_transparent_color);
        indicator_3->set_foreground_color(k_control_transparent_color);
        indicator_3->remove_component(gb::ces_bound_touch_component::class_guid());
        
        dialog->add_control(label, game::ces_ui_interaction_component::k_slow_motion_boost_trigger_dialog_label);
        dialog->add_control(indicator_1, game::ces_ui_interaction_component::k_slow_motion_boost_trigger_dialog_indicator_1);
        // dialog->add_control(indicator_2, game::ces_ui_interaction_component::k_slow_motion_boost_trigger_dialog_indicator_2);
        dialog->add_control(indicator_3, game::ces_ui_interaction_component::k_slow_motion_boost_trigger_dialog_indicator_3);
        dialog->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_slow_motion_boost_trigger_dialog)];
        
        return dialog;
    }

#if defined(__OSX__)

    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_controls_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_controls_dialog);
        
        dialog->set_title("CONTROLS");
        dialog->set_title_text_color(k_control_text_color);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(400.f, 24.f), "YOU CAN CONTROL YOUR CAR USING        MOUSE / TOUCHPAD OR USING KEYBOARD W, A, S, D.");
        
        label->position = glm::vec2(0.f, 0.f);
        label->set_text_color(k_control_text_color);
        label->set_font_name("spincycle.otf");
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        label->set_multiline(true);
        
        const auto ok_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        ok_button->position = glm::vec2(400.f * .5f - k_image_button_size.x * .5f, 50.f);
        colorize_ui_control(ok_button);
        ok_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        dialog->add_control(label, game::ces_ui_interaction_component::k_controls_dialog_description_label);
        dialog->add_control(ok_button, game::ces_ui_interaction_component::k_controls_dialog_ok_button);
        dialog->visible = false;
        glm::vec2 dialog_size = dialog->size;
               dialog->position = glm::vec2(m_screen_size.x * .5f - dialog_size.x * .5f,
                                            m_screen_size.y * .5f - dialog_size.y * .5f);
        dialog->set_background_color(k_control_background_color);
        
        return dialog;
    }

#endif

    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_daily_tasks_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_target.png", nullptr);
        add_ui_interaction_component(button, game::ces_ui_interaction_component::e_ui::e_ui_daily_tasks_button);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_daily_tasks_button)];
               colorize_ui_control(button);
         button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
         return button;
    }

    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_daily_tasks_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "DAILY TASK");
        add_ui_interaction_component(label, game::ces_ui_interaction_component::e_ui::e_ui_daily_tasks_label);
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_daily_tasks_label)];
        colorize_ui_control(label);
        label->set_font_size(24.f);
        label->set_text_vertical_aligment(gb::ui::e_element_vertical_aligment_center);
        label->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        return label;
    }

    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_daily_tasks_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        colorize_ui_control(dialog);
        dialog->visible = false;
        dialog->show_title(false);
        add_ui_interaction_component(dialog, game::ces_ui_interaction_component::e_ui::e_ui_daily_tasks_dialog);
        
        auto table_view = m_ui_base_fabricator.lock()->create_table_view(glm::vec2(300.f,
                                                                                   m_screen_size.y - 56.f * 2.f));
        table_view->position = glm::vec2(0.f);
        table_view->set_background_color(k_control_transparent_color);
        dialog->add_control(table_view, game::ces_ui_interaction_component::k_daily_tasks_dialog_table_view);
        dialog->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_daily_tasks_dialog)];
        
        return dialog;
    }

}
