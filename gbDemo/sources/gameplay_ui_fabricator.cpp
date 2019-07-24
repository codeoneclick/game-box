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
#include "ces_car_model_component.h"

namespace game
{
    
#if defined(__OSX__)
    
    const glm::vec2 gameplay_ui_fabricator::k_image_button_size = glm::vec2(96.f);
    
#else
    
    const glm::vec2 gameplay_ui_fabricator::k_image_button_size = glm::vec2(48.f);
    
#endif
    
    const glm::u8vec4 gameplay_ui_fabricator::k_font_color = glm::u8vec4(214, 214, 157, 255);
    const glm::u8vec4 gameplay_ui_fabricator::k_image_button_color = glm::u8vec4(214, 157, 214, 255);
    const glm::u8vec4 gameplay_ui_fabricator::k_image_button_background_color = glm::u8vec4(26, 47, 52, 127);
    const glm::u8vec4 gameplay_ui_fabricator::k_image_button_foreground_color = glm::u8vec4(0, 0, 0, 127);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_background_color = glm::u8vec4(26, 47, 52, 127);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_foreground_color = glm::u8vec4(26, 47, 52, 127);
    const glm::u8vec4 gameplay_ui_fabricator::k_control_transparent_color = glm::u8vec4(0);
    
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
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_quit_game_button)] = glm::vec2(32.f, 40.f + 8.f * 2.f + k_image_button_size.y * 2.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_facebook_button)] = glm::vec2(32.f, 40.f + 8.f * 3.f + k_image_button_size.y * 3.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_back_from_garage_button)] = glm::vec2(32.f, 40.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_back_from_garage_label)] = glm::vec2(32.f + k_image_button_size.x, 40.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_garage_preview_button)] = glm::vec2(32.f, 40.f + 8.f + k_image_button_size.y);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_apply_color_1_button)] = glm::vec2(32.f + 120.f, 40.f + 8.f + k_image_button_size.y);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_apply_color_2_button)] = glm::vec2(32.f + 120.f + (8.f + k_image_button_size.x), 40.f + 8.f + k_image_button_size.y);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_apply_color_3_button)] = glm::vec2(32.f + 120.f + (8.f + k_image_button_size.x) * 2.f, 40.f + 8.f + k_image_button_size.y);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_apply_color_4_button)] = glm::vec2(32.f + 120.f + (8.f + k_image_button_size.x) * 3.f, 40.f + 8.f + k_image_button_size.y);
        
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_next_car_in_garage_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f, m_screen_size.y - k_image_button_size.y - 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_prev_car_in_garage_button)] = glm::vec2(32.f, m_screen_size.y - k_image_button_size.y - 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_skin_1_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 40.f, 40.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_skin_2_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 40.f, 40.f + k_image_button_size.y + 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_skin_3_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 40.f, 40.f + k_image_button_size.y + 8.f + k_image_button_size.y + 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_left_label)] = glm::vec2(32.f, m_screen_size.y * .5f + 24.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_right_label)] = glm::vec2(m_screen_size.x - 128.f, m_screen_size.y * .5f + 24.f);
         ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_label)] = glm::vec2(m_screen_size.x * .5f - 200.f, m_screen_size.y * .5f - 300.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_goto_racing1_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f, m_screen_size.y - k_image_button_size.y - 8.f);
         ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_goto_racing2_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f - k_image_button_size.x, m_screen_size.y - k_image_button_size.y - 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_pause_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f, 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_tickets_label)] = glm::vec2(32.f, 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_select_car_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f - 8.f - k_image_button_size.x, m_screen_size.y - k_image_button_size.y - 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_unlock_car_button)] = glm::vec2(m_screen_size.x - k_image_button_size.x - 32.f - 8.f - k_image_button_size.x, m_screen_size.y - k_image_button_size.y - 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_bar)] = glm::vec2(m_screen_size.x - 32.f - 210.f, 40.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_label)] = glm::vec2(m_screen_size.x - 32.f - 210.f, 8.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_info_label)] = glm::vec2(m_screen_size.x - 32.f - 105.f, 64.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_rank_info_label)] = glm::vec2(m_screen_size.x - 32.f - 210.f, 64.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_button)] = glm::vec2(m_screen_size.x - 32.f - 210.f - 8.f - k_image_button_size.x, 8.f);
        
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_damage_bar)] = glm::vec2(m_screen_size.x - 32.f - 150.f - 8.f - k_image_button_size.x, 32.f);
        ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_damage_label)] = glm::vec2(m_screen_size.x - 32.f - 150.f - 8.f - k_image_button_size.x, 8.f);
    }
    
    glm::ivec2 gameplay_ui_fabricator::get_screen_size() const
    {
        return m_screen_size;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_open_levels_list_dialog_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_trophy.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_open_levels_list_dialog_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_open_levels_list_dialog_button);
        button->add_component(ui_interaction_component);

        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_open_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_gear.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_open_garage_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_open_garage_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
#if defined(__IOS__)
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_open_shop_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_cart.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_open_shop_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_open_shop_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
#endif
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_back_from_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_prev.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_back_from_garage_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_back_from_garage_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_levels_list_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(100.f));
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_levels_list_dialog);
        dialog->add_component(ui_interaction_component);
        
        dialog->show_title(false);
        
        auto levels_list_table_view = m_ui_base_fabricator.lock()->create_table_view(glm::vec2(256.f,
                                                                                               m_screen_size.y - 48.f));
        levels_list_table_view->position = glm::vec2(40.f + k_image_button_size.x, 40.f);
        levels_list_table_view->set_background_color(glm::u8vec4(0, 0, 0, 32));
        dialog->add_control(levels_list_table_view, game::ces_ui_interaction_component::k_levels_list_dialog_levels_table);
        dialog->visible = false;
        dialog->set_background_color(glm::u8vec4(0));
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_countdown_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(220.f, 24.f), "TIME LEFT: 00:00 sec");
        label->position = glm::vec2(m_screen_size.x * .5f - 220.f * .5f,
                                    8.f);
        label->set_background_color(glm::u8vec4(0, 0, 0, 0));
        label->set_foreground_color(glm::u8vec4(0, 0, 0, 0));
        label->set_font_color(k_font_color);
        label->visible = false;
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_countdown_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_next_car_in_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_prev.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_prev_car_in_garage_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_prev_car_in_garage_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_prev_car_in_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_next.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_next_car_in_garage_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_next_car_in_garage_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_skin_1_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_1.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_skin_1_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_car_skin_1_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_skin_2_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_2.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_skin_2_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_car_skin_2_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_skin_3_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_3.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_skin_3_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_car_skin_3_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_cars_list_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(100.f));
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_cars_list_dialog);
        dialog->add_component(ui_interaction_component);
        
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
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_left_label)];
        label->set_font_color(k_font_color);
        label->set_visible_edges(false);
        label->set_font_mode(gb::ces_font_component::e_font_mode_edge);
        label->remove_component(gb::ces_bound_touch_component::class_guid());
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_left_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_tutorial_steer_right_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(96.f, 192.f), "PRESS");
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_right_label)];
        label->set_font_color(k_font_color);
        label->set_visible_edges(false);
        label->set_font_mode(gb::ces_font_component::e_font_mode_edge);
        label->remove_component(gb::ces_bound_touch_component::class_guid());
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_right_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_goto_racing1_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_next.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_goto_racing1_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_goto_racing1_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_goto_racing2_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_button(k_image_button_size, nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_goto_racing2_button)];
        button->set_text("GO");
        button->set_text_color(k_font_color);
        button->set_background_color(k_image_button_background_color);
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_goto_racing2_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_in_game_pause_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_pause.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_pause_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_pause_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_in_game_pause_menu_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(100.f));
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_pause_menu_dialog);
        dialog->add_component(ui_interaction_component);
        
        dialog->set_title("PAUSE");
        dialog->set_title_text_color(k_font_color);
        
        f32 x_offset = 8.f;
        const auto continue_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_next.png", nullptr);
        continue_button->position = glm::vec2(x_offset, 8.f);
        continue_button->set_image_color(k_image_button_color);
        continue_button->set_background_color(k_image_button_background_color);
        continue_button->set_foreground_color(k_image_button_foreground_color);
        continue_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        x_offset += 16.f + k_image_button_size.x;
        
        const auto restart_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_restart.png", nullptr);
        restart_button->position = glm::vec2(x_offset, 8.f);
        restart_button->set_image_color(k_image_button_color);
        restart_button->set_background_color(k_image_button_background_color);
        restart_button->set_foreground_color(k_image_button_foreground_color);
        restart_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        x_offset += 16.f + k_image_button_size.x;
        
        const auto exit_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_exit.png", nullptr);
        exit_button->position = glm::vec2(x_offset, 8.f);
        exit_button->set_image_color(k_image_button_color);
        exit_button->set_background_color(k_image_button_background_color);
        exit_button->set_foreground_color(k_image_button_foreground_color);
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
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_restart_dialog);
        dialog->add_component(ui_interaction_component);
        
        dialog->set_title("RESTART ?");
        dialog->set_title_text_color(k_font_color);
        
        std::string message = "";
        
#if defined(__IOS__)
        
        message = "ARE YOU SURE ? IF YOU RESTART THE LEVEL YOU WILL MISS YOUR CURRENT PROGRESS AND ONE TICKET.";
        
#else
        
        message = "ARE YOU SURE ? IF YOU RESTART THE LEVEL YOU WILL MISS YOUR CURRENT PROGRESS.";
        
#endif
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(400.f, 128.f), message);
        
        label->position = glm::vec2(0.f, -24.f);
        label->set_font_color(k_font_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        label->set_font_size(20.f);
        label->set_multiline(true);
        
        const auto yes_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        yes_button->position = glm::vec2(400.f * .5f - k_image_button_size.x * 2.f, 80.f);
        yes_button->set_image_color(glm::u8vec4(64, 64, 255, 255));
        yes_button->set_background_color(glm::u8vec4(0, 96, 0, 96));
        yes_button->set_foreground_color(k_image_button_foreground_color);
        yes_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto no_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_cross.png", nullptr);
        no_button->position = glm::vec2(400.f * .5f + k_image_button_size.x, 80.f);
        no_button->set_image_color(glm::u8vec4(255, 64, 64, 255));
        no_button->set_background_color(glm::u8vec4(96, 0, 0, 96));
        no_button->set_foreground_color(k_image_button_foreground_color);
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
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_quit_dialog);
        dialog->add_component(ui_interaction_component);
        
        dialog->set_title("QUIT ?");
        dialog->set_title_text_color(k_font_color);
        
        std::string message = "";
        
#if defined(__IOS__)
        
        message = "ARE YOU SURE ? IF YOU QUIT FROM RACE YOU WILL MISS YOUR CURRENT PROGRESS AND ONE TICKET.";
        
#else
        
        message = "ARE YOU SURE ? IF YOU QUIT FROM RACE YOU WILL MISS YOUR CURRENT PROGRESS.";
        
#endif
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(420.f, 128.f), message);
        
        label->position = glm::vec2(0.f, -24.f);
        label->set_font_color(k_font_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        label->set_font_size(20.f);
        label->set_multiline(true);
        
        const auto yes_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        yes_button->position = glm::vec2(420.f * .5f - k_image_button_size.x * 2.f, 80.f);
        yes_button->set_image_color(glm::u8vec4(64, 64, 255, 255));
        yes_button->set_background_color(glm::u8vec4(0, 96, 0, 96));
        yes_button->set_foreground_color(k_image_button_foreground_color);
        yes_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto no_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_cross.png", nullptr);
        no_button->position = glm::vec2(420.f * .5f + k_image_button_size.x, 80.f);
        no_button->set_image_color(glm::u8vec4(255, 64, 64, 255));
        no_button->set_background_color(glm::u8vec4(96, 0, 0, 96));
        no_button->set_foreground_color(k_image_button_foreground_color);
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
        const auto tickets_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(240.f, 24.f), "TICKETS: 3");
        tickets_label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_tickets_label)];
        tickets_label->set_font_color(k_font_color);
        tickets_label->set_foreground_color(k_control_foreground_color);
        tickets_label->set_background_color(k_control_background_color);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_tickets_label);
        tickets_label->add_component(ui_interaction_component);
        
        const auto plus_ticket_button =  m_ui_base_fabricator.lock()->create_button(glm::vec2(32.f, 24.f), nullptr);
        ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_tickets_plus_button);
        plus_ticket_button->add_component(ui_interaction_component);
        
        plus_ticket_button->position = glm::vec2(208.f, 0.f);
        plus_ticket_button->set_background_color(k_control_background_color);
        plus_ticket_button->set_text("+");
        plus_ticket_button->set_text_color(k_font_color);
        plus_ticket_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        tickets_label->add_child(plus_ticket_button);
        
        return tickets_label;
    }
    
#endif
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_win_dialog(const std::string& filename)
    {
        auto dialog =  m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_win_dialog);
        dialog->add_component(ui_interaction_component);
        
        dialog->set_title("WIN");
        dialog->set_title_text_color(k_font_color);
        
        const auto place_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(450.f, 24.f), "FINISHED AT PLACE: SECOND");
        
        place_label->position = glm::vec2(0.f, -64.f);
        place_label->set_font_color(k_font_color);
        place_label->set_background_color(k_control_background_color);
        place_label->set_foreground_color(k_control_foreground_color);
        place_label->set_visible_edges(false);
        place_label->set_background_color(glm::u8vec4(96, 96, 96, 96));
        
        const auto drift_time_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(450.f, 24.f), "DRIFT TIME: 19:45 sec");
        
        drift_time_label->position = glm::vec2(0.f, -32.f);
        drift_time_label->set_font_color(k_font_color);
        drift_time_label->set_background_color(k_control_background_color);
        drift_time_label->set_foreground_color(k_control_foreground_color);
        drift_time_label->set_visible_edges(false);
        drift_time_label->set_background_color(glm::u8vec4(96, 96, 96, 96));
        
        const auto star1_image = m_general_fabricator.lock()->create_sprite("ui_image.xml", "ui_star.png");
        star1_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
        star1_image->size = glm::vec2(64.f, 64.f);
        star1_image->color = glm::u8vec4(192, 0, 192, 255);
        star1_image->position = glm::vec2(450.f * .5f - 75.f - 64.f * .5f, -4.f);
        
        const auto star1_achievement_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(150.f, 24.f), "LOW DAMAGE!");
        
        star1_achievement_label->position = glm::vec2(450.f * .5f - 150.f - 75.f, 68.f);
        star1_achievement_label->set_font_color(k_font_color);
        star1_achievement_label->set_background_color(k_control_transparent_color);
        star1_achievement_label->set_foreground_color(k_control_transparent_color);
        star1_achievement_label->set_visible_edges(false);
        star1_achievement_label->set_font_size(20.f);
        
        const auto star2_image = m_general_fabricator.lock()->create_sprite("ui_image.xml", "ui_star.png");
        star2_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
        star2_image->size = glm::vec2(64.f, 64.f);
        star2_image->color = glm::u8vec4(192, 0, 192, 255);
        star2_image->position = glm::vec2(450.f * .5f - 64.f * .5f, -4.f);
        
        const auto star2_achievement_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(150.f, 24.f), "FIRST PLACE!");
        
        star2_achievement_label->position = glm::vec2(450.f * .5f - 150.f * .5f, 68.f);
        star2_achievement_label->set_font_color(k_font_color);
        star2_achievement_label->set_background_color(k_control_transparent_color);
        star2_achievement_label->set_foreground_color(k_control_transparent_color);
        star2_achievement_label->set_visible_edges(false);
        star2_achievement_label->set_font_size(20.f);
        
        const auto star3_image = m_general_fabricator.lock()->create_sprite("ui_image.xml", "ui_star.png");
        star3_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
        star3_image->size = glm::vec2(64.f, 64.f);
        star3_image->color = glm::u8vec4(32, 32, 32, 255);
        star3_image->position = glm::vec2(450.f * .5f + 75.f * .5, -4.f);
        
        const auto star3_achievement_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(150.f, 24.f), "GOOD DRIFT!");
        
        star3_achievement_label->position = glm::vec2(450.f * .5f + 150.f * .5f, 68.f);
        star3_achievement_label->set_font_color(k_font_color);
        star3_achievement_label->set_background_color(k_control_transparent_color);
        star3_achievement_label->set_foreground_color(k_control_transparent_color);
        star3_achievement_label->set_visible_edges(false);
        star3_achievement_label->set_font_size(20.f);
        
        const auto restart_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_restart.png", nullptr);
        restart_button->position = glm::vec2(450.f * .5f - k_image_button_size.x * 2.f, 100.f);
        restart_button->set_image_color(k_image_button_color);
        restart_button->set_background_color(k_image_button_background_color);
        restart_button->set_foreground_color(k_image_button_foreground_color);
        restart_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto continue_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_next.png", nullptr);
        continue_button->position = glm::vec2(450.f * .5f + k_image_button_size.x, 100.f);
        continue_button->set_image_color(k_image_button_color);
        continue_button->set_background_color(k_image_button_background_color);
        continue_button->set_foreground_color(k_image_button_foreground_color);
        continue_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        dialog->add_control(place_label, game::ces_ui_interaction_component::k_end_game_dialog_place_label);
        dialog->add_control(drift_time_label, game::ces_ui_interaction_component::k_end_game_dialog_drift_time_label);
        dialog->add_control(star1_image, game::ces_ui_interaction_component::k_end_game_dialog_star1_image);
        dialog->add_control(star2_image, game::ces_ui_interaction_component::k_end_game_dialog_star2_image);
        dialog->add_control(star3_image, game::ces_ui_interaction_component::k_end_game_dialog_star3_image);
        dialog->add_control(star1_achievement_label, game::ces_ui_interaction_component::k_win_dialog_star1_achievement_label);
        dialog->add_control(star2_achievement_label, game::ces_ui_interaction_component::k_win_dialog_star2_achievement_label);
        dialog->add_control(star3_achievement_label, game::ces_ui_interaction_component::k_win_dialog_star3_achievement_label);
        dialog->add_control(continue_button, game::ces_ui_interaction_component::k_end_game_dialog_continue_button);
        dialog->add_control(restart_button, game::ces_ui_interaction_component::k_end_game_dialog_restart_button);
        
        dialog->visible = false;
        dialog->position = glm::vec2(m_screen_size.x * .5f - 450.f * .5f, m_screen_size.y * .5 - 48.f);
        dialog->set_background_color(k_control_background_color);
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_loose_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_loose_dialog);
        dialog->add_component(ui_interaction_component);
        
        dialog->set_title("LOOSE");
        dialog->set_title_text_color(k_font_color);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(420.f, 128.f), "YOUR CAR IS CRASHED");
        
        label->position = glm::vec2(0.f, -30.f);
        label->set_font_color(k_font_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        label->set_visible_edges(false);
        
        const auto restart_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_restart.png", nullptr);
        restart_button->position = glm::vec2(420.f * .5f - k_image_button_size.x * 2.f, 80.f);
        restart_button->set_image_color(k_image_button_color);
        restart_button->set_background_color(k_image_button_background_color);
        restart_button->set_foreground_color(k_image_button_foreground_color);
        restart_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto continue_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_next.png", nullptr);
        continue_button->position = glm::vec2(420.f * .5f + k_image_button_size.x, 80.f);
        continue_button->set_image_color(k_image_button_color);
        continue_button->set_background_color(k_image_button_background_color);
        continue_button->set_foreground_color(k_image_button_foreground_color);
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
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_select_car_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_select_car_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_unlock_car_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_locked.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_unlock_car_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_unlock_car_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_stars_progress_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(210.f, 24.f), "STARS PROGRESS:");
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_label)];
        label->set_font_color(k_font_color);
        label->set_foreground_color(k_control_foreground_color);
        label->set_background_color(k_control_background_color);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_stars_progress_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_stars_progress_info_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(105.f, 24.f), "0/3:");
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_info_label)];
        label->set_font_color(k_font_color);
        label->set_font_mode(gb::ces_font_component::e_font_mode_edge);
        label->set_foreground_color(k_control_foreground_color);
        label->set_background_color(k_control_background_color);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_stars_progress_info_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_stars_progress_bar(const std::string& filename)
    {
        const auto progress_bar = m_ui_base_fabricator.lock()->create_progress_bar(glm::vec2(210.f, 24.f));
        progress_bar->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_bar)];
        progress_bar->set_progress_line_color(k_font_color);
        progress_bar->set_background_color(k_control_background_color);

        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_stars_progress_bar);
        progress_bar->add_component(ui_interaction_component);
        
        return progress_bar;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_stars_progress_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_star.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_stars_progress_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_stars_progress_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_rank_info_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(105.f, 24.f), "RANK: 1");
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_rank_info_label)];
        label->set_font_color(k_font_color);
        label->set_font_mode(gb::ces_font_component::e_font_mode_edge);
        label->set_foreground_color(k_control_foreground_color);
        label->set_background_color(k_control_background_color);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_rank_info_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_damage_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(150.f, 24.f), "DAMAGE:");
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_damage_label)];
        label->set_font_color(k_font_color);
        label->set_foreground_color(k_control_foreground_color);
        label->set_background_color(k_control_background_color);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_car_damage_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_damage_bar(const std::string& filename)
    {
        const auto progress_bar = m_ui_base_fabricator.lock()->create_progress_bar(glm::vec2(150.f, 24.f));
        progress_bar->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_car_damage_bar)];
        progress_bar->set_progress_line_color(k_font_color);
        progress_bar->set_background_color(k_control_background_color);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_car_damage_bar);
        progress_bar->add_component(ui_interaction_component);
        
        return progress_bar;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_full_tickets_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(100.f));
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_full_tickets_dialog);
        dialog->add_component(ui_interaction_component);
        
        dialog->set_title("TICKETS");
        dialog->set_title_text_color(k_font_color);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(400.f, 24.f), "YOU ALREADY HAVE 5 TICKETS!");
        
        label->position = glm::vec2(0.f, -50.f);
        label->set_font_color(k_font_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        
        const auto ok_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        ok_button->position = glm::vec2(400.f * .5f - 24.f, -20.f);
        ok_button->set_image_color(k_image_button_color);
        ok_button->set_background_color(k_image_button_background_color);
        ok_button->set_foreground_color(k_image_button_foreground_color);
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
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_not_enough_tickets_dialog);
        dialog->add_component(ui_interaction_component);
        
        dialog->set_title("TICKETS");
        dialog->set_title_text_color(k_font_color);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(400.f, 24.f), "NOT ENOUGH TICKETS TO START RACE!");
        
        label->position = glm::vec2(0.f, -50.f);
        label->set_font_color(k_font_color);
        label->set_background_color(k_control_background_color);
        label->set_foreground_color(k_control_foreground_color);
        
        const auto ok_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        ok_button->position = glm::vec2(400.f * .5f - 24.f - 32.f, -20.f);
        ok_button->set_image_color(k_image_button_color);
        ok_button->set_background_color(k_image_button_background_color);
        ok_button->set_foreground_color(k_image_button_foreground_color);
        ok_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto plus_ticket_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_video.png", nullptr);
        plus_ticket_button->position = glm::vec2(400.f * .5f - 24.f + 32.f, -20.f);
        plus_ticket_button->set_image_color(k_image_button_color);
        plus_ticket_button->set_background_color(k_image_button_background_color);
        plus_ticket_button->set_foreground_color(k_image_button_foreground_color);
        plus_ticket_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);

        dialog->add_control(label, game::ces_ui_interaction_component::k_not_enough_tickets_dialog_label);
        dialog->add_control(ok_button, game::ces_ui_interaction_component::k_not_enough_tickets_dialog_ok_button);
        dialog->add_control(plus_ticket_button, game::ces_ui_interaction_component::k_not_enough_tickets_dialog_plus_button);
        dialog->visible = false;
        dialog->position = glm::vec2(m_screen_size.x * .5f - 400.f * .5f, m_screen_size.y * .5 - 48.f);
        dialog->set_background_color(k_control_background_color);
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_shop_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(100.f));
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_shop_dialog);
        dialog->add_component(ui_interaction_component);
        
        dialog->show_title(false);
        
        auto shop_table_view = m_ui_base_fabricator.lock()->create_table_view(glm::vec2(256.f,
                                                                                        m_screen_size.y - 48.f));
        shop_table_view->position = glm::vec2(80.f, 40.f);
        shop_table_view->set_background_color(glm::u8vec4(0, 0, 0, 32));
        dialog->add_control(shop_table_view, game::ces_ui_interaction_component::k_shop_dialog_table_view);
        dialog->visible = false;
        dialog->set_background_color(glm::u8vec4(0));
        
        return dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_screen_overlay(const std::string& filename)
    {
        const auto overlay = m_general_fabricator.lock()->create_sprite("button_background.xml");
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_screen_overlay);
        overlay->add_component(ui_interaction_component);
        overlay->color = glm::u8vec4(0, 0, 0, 192);
        overlay->position = glm::vec2(-m_screen_size.x * .5f, -m_screen_size.y * .5f);
        overlay->size = glm::vec2(m_screen_size.x, m_screen_size.y);
        overlay->visible = false;
        return overlay;
    }
    
#if defined(__FACEBOOK_LOGIN__)
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_facebook_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_facebook.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_facebook_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_facebook_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
#endif
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_unlock_car_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(100.f));
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_unlock_car_dialog);
        dialog->add_component(ui_interaction_component);
        
        dialog->set_title("UNLOCK");
        dialog->set_title_text_color(k_font_color);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(400.f, 24.f), "CAR WILL BE UNLOCKED AT RANK 0");
        
        label->position = glm::vec2(0.f, 0.f);
        label->set_font_color(k_font_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        
        const auto ok_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        ok_button->position = glm::vec2(400.f * .5f - k_image_button_size.x * .5f, 50.f);
        ok_button->set_image_color(k_image_button_color);
        ok_button->set_background_color(k_image_button_background_color);
        ok_button->set_foreground_color(k_image_button_foreground_color);
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
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(400.f, 200.f), "PRESS IN FRONT OF THE CAR AND MOVE IN THE DIRECTION OF THE TURN");
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_label)];
        label->set_font_color(k_font_color);
        label->set_visible_edges(false);
        label->set_font_mode(gb::ces_font_component::e_font_mode_edge);
        label->remove_component(gb::ces_bound_touch_component::class_guid());
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
#if defined(__OSX__)
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_exit_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_exit.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_quit_game_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_quit_game_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_exit_game_dialog(const std::string& filename)
    {
        auto dialog = m_ui_base_fabricator.lock()->create_dialog(glm::vec2(0.f));
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_quit_game_dialog);
        dialog->add_component(ui_interaction_component);
        
        dialog->set_title("QUIT ?");
        dialog->set_title_text_color(k_font_color);
        
        std::string message = "ARE YOU SURE ?";
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(420.f, 128.f), message);
        
        label->position = glm::vec2(0.f, -24.f);
        label->set_font_color(k_font_color);
        label->set_background_color(k_control_transparent_color);
        label->set_foreground_color(k_control_transparent_color);
        
        const auto yes_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_checkmark.png", nullptr);
        yes_button->position = glm::vec2(420.f * .5f - k_image_button_size.x * 2.f, 80.f);
        yes_button->set_image_color(glm::u8vec4(64, 64, 255, 255));
        yes_button->set_background_color(glm::u8vec4(0, 96, 0, 96));
        yes_button->set_foreground_color(k_image_button_foreground_color);
        yes_button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        const auto no_button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_cross.png", nullptr);
        no_button->position = glm::vec2(420.f * .5f + k_image_button_size.x, 80.f);
        no_button->set_image_color(glm::u8vec4(255, 64, 64, 255));
        no_button->set_background_color(glm::u8vec4(96, 0, 0, 96));
        no_button->set_foreground_color(k_image_button_foreground_color);
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
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_career_label)];
        label->set_font_color(k_font_color);
        label->set_foreground_color(k_control_foreground_color);
        label->set_background_color(k_control_background_color);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_career_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_garage_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "GARAGE");
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_garage_label)];
        label->set_font_color(k_font_color);
        label->set_foreground_color(k_control_foreground_color);
        label->set_background_color(k_control_background_color);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_garage_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_back_from_garage_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "BACK");
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_back_from_garage_label)];
        label->set_font_color(k_font_color);
        label->set_foreground_color(k_control_foreground_color);
        label->set_background_color(k_control_background_color);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_back_from_garage_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
#if defined(__IOS__)
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_shop_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(160.f, k_image_button_size.y), "SHOP");
        label->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_shop_label)];
        label->set_font_color(k_font_color);
        label->set_foreground_color(k_control_foreground_color);
        label->set_background_color(k_control_background_color);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_shop_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
#endif
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_garage_preview_mode_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_search.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_garage_preview_button)];
        button->set_image_color(k_image_button_color);
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_garage_preview_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_apply_color_1_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_color.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_apply_color_1_button)];
        button->set_image_color(glm::u8vec4(ces_car_model_component::k_car_color_1.x,
                                            ces_car_model_component::k_car_color_1.z,
                                            ces_car_model_component::k_car_color_1.y,
                                            ces_car_model_component::k_car_color_1.w));
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_apply_color_1_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_apply_color_2_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_color.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_apply_color_2_button)];
        button->set_image_color(glm::u8vec4(ces_car_model_component::k_car_color_2.x,
                                            ces_car_model_component::k_car_color_2.z,
                                            ces_car_model_component::k_car_color_2.y,
                                            ces_car_model_component::k_car_color_2.w));
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_apply_color_2_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_apply_color_3_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_color.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_apply_color_3_button)];
        button->set_image_color(glm::u8vec4(ces_car_model_component::k_car_color_3.x,
                                            ces_car_model_component::k_car_color_3.z,
                                            ces_car_model_component::k_car_color_3.y,
                                            ces_car_model_component::k_car_color_3.w));
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_apply_color_3_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }

    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_apply_color_4_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(k_image_button_size, "ui_color.png", nullptr);
        button->position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_apply_color_4_button)];
        button->set_image_color(glm::u8vec4(ces_car_model_component::k_car_color_4.x,
                                            ces_car_model_component::k_car_color_4.z,
                                            ces_car_model_component::k_car_color_4.y,
                                            ces_car_model_component::k_car_color_4.w));
        button->set_background_color(k_image_button_background_color);
        button->set_foreground_color(k_image_button_foreground_color);
        button->attach_sound("click.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui::e_ui_apply_color_4_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
}
