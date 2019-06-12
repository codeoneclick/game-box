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
#include "anim_fabricator.h"
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
#include "advertisement_provider.h"

namespace game
{
    glm::vec2 gameplay_ui_fabricator::k_open_levels_list_dialog_button_position = glm::vec2(8.f, 40.f);
    glm::vec2 gameplay_ui_fabricator::k_open_garage_button_position = glm::vec2(8.f, 96.f);
    glm::vec2 gameplay_ui_fabricator::k_back_from_garage_button_position = glm::vec2(8.f, 40.f);
    glm::vec2 gameplay_ui_fabricator::k_next_car_in_garage_button_position = glm::vec2(0.f, 0.f);
    glm::vec2 gameplay_ui_fabricator::k_prev_car_in_garage_button_position = glm::vec2(0.f, 0.f);
    glm::vec2 gameplay_ui_fabricator::k_car_skin_1_button_position = glm::vec2(0.f, 0.f);
    glm::vec2 gameplay_ui_fabricator::k_car_skin_2_button_position = glm::vec2(0.f, 0.f);
    glm::vec2 gameplay_ui_fabricator::k_car_skin_3_button_position = glm::vec2(0.f, 0.f);
    glm::vec2 gameplay_ui_fabricator::k_tutorial_steer_left_label_position = glm::vec2(0.f, 0.f);
    glm::vec2 gameplay_ui_fabricator::k_tutorial_steer_right_label_position = glm::vec2(0.f, 0.f);
    glm::vec2 gameplay_ui_fabricator::k_goto_racing_button_position = glm::vec2(0.f, 0.f);
    glm::vec2 gameplay_ui_fabricator::k_pause_button_position = glm::vec2(0.f, 0.f);
    glm::vec2 gameplay_ui_fabricator::k_tickets_label_position = glm::vec2(0.f, 0.f);
    
    gameplay_ui_fabricator::gameplay_ui_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator,
                                                   const gb::ui::ui_fabricator_shared_ptr& ui_base_fabricator,
                                                   const glm::ivec2& screen_size) :
    m_general_fabricator(general_fabricator),
    m_ui_base_fabricator(ui_base_fabricator),
    m_screen_size(screen_size)
    {
        k_next_car_in_garage_button_position.x = m_screen_size.x - 48.f - 8.f;
        k_next_car_in_garage_button_position.y = m_screen_size.y - 48.f - 8.f;
        k_prev_car_in_garage_button_position.x = 8.f;
        k_prev_car_in_garage_button_position.y = m_screen_size.y - 48.f - 8.f;
        
        k_car_skin_1_button_position.x = m_screen_size.x - 48.f - 8;
        k_car_skin_1_button_position.y = 96.f;
        
        k_car_skin_2_button_position.x = m_screen_size.x - 48.f - 8;
        k_car_skin_2_button_position.y = k_car_skin_1_button_position.y + 48.f + 8.f;
        
        k_car_skin_3_button_position.x = m_screen_size.x - 48.f - 8;
        k_car_skin_3_button_position.y = k_car_skin_2_button_position.y + 48.f + 8.f;
        
        k_tutorial_steer_left_label_position.x = 32.f;
        k_tutorial_steer_left_label_position.y = m_screen_size.y * .5f + 24.f;
        
        k_tutorial_steer_right_label_position.x = m_screen_size.x - 128.f;
        k_tutorial_steer_right_label_position.y = m_screen_size.y * .5f + 24.f;
        
        k_goto_racing_button_position.x = m_screen_size.x - 48.f - 8.f;
        k_goto_racing_button_position.y = m_screen_size.y - 48.f - 8.f;
        
        k_pause_button_position.x = m_screen_size.x - 48.f - 8.f;
        k_pause_button_position.y = 8.f;
        
        k_tickets_label_position.x = 8.f;
        k_tickets_label_position.y = 8.f;
    }
    
    glm::ivec2 gameplay_ui_fabricator::get_screen_size() const
    {
        return m_screen_size;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_open_levels_list_dialog_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_trophy.png", nullptr);
        button->position = k_open_levels_list_dialog_button_position;
        button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_open_levels_list_dialog_button);
        button->add_component(ui_interaction_component);

        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_open_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_gear.png", nullptr);
        button->position = k_open_garage_button_position;
        button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_open_garage_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_back_from_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_prev.png", nullptr);
        button->position = k_back_from_garage_button_position;
        button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_back_from_garage_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_levels_list_dialog(const std::string& filename)
    {
        auto levels_list_dialog = gb::ces_entity::construct<gb::ui::dialog>();
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_levels_list_dialog);
        levels_list_dialog->add_component(ui_interaction_component);
        
        auto levels_list_table_view = m_ui_base_fabricator.lock()->create_table_view(glm::vec2(256.f,
                                                                                               m_screen_size.y - 48.f));
        levels_list_table_view->position = glm::vec2(64.f, 40.f);
        levels_list_table_view->set_background_color(glm::u8vec4(0, 0, 0, 32));
        levels_list_dialog->add_control(levels_list_table_view, game::ces_ui_interaction_component::k_levels_list_dialog_levels_table);
        levels_list_dialog->visible = false;
        
        return levels_list_dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_scores_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(200.f, 24.f), "SCORES: 0");
        label->position = glm::vec2(460.f, 8.f);
        label->set_font_color(glm::u8vec4(255, 255, 255, 255));
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_scores_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_countdown_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(220.f, 24.f), "TIME LEFT: 00:00 sec");
        label->position = glm::vec2(m_screen_size.x * .5f - 220.f * .5f,
                                    8.f);
        label->set_background_color(glm::u8vec4(0, 0, 0, 32));
        label->set_foreground_color(glm::u8vec4(0, 0, 0, 32));
        label->set_font_color(glm::u8vec4(255, 255, 255, 255));
        label->visible = false;
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_countdown_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_next_car_in_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_prev.png", nullptr);
        button->position = k_next_car_in_garage_button_position;
        button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_prev_car_in_garage_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_prev_car_in_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_next.png", nullptr);
        button->position = k_prev_car_in_garage_button_position;
        button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_next_car_in_garage_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_skin_1_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_1.png", nullptr);
        button->position = k_car_skin_1_button_position;
        button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_car_skin_1_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_skin_2_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_2.png", nullptr);
        button->position = k_car_skin_2_button_position;
        button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_car_skin_2_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_car_skin_3_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_3.png", nullptr);
        button->position = k_car_skin_3_button_position;
        button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_car_skin_3_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_cars_list_dialog(const std::string& filename)
    {
        auto cars_list_dialog = gb::ces_entity::construct<gb::ui::dialog>();
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_cars_list_dialog);
        cars_list_dialog->add_component(ui_interaction_component);
        
        auto cars_list_table_view = m_ui_base_fabricator.lock()->create_table_view(glm::vec2(130.f,
                                                                                             160.f));
        cars_list_table_view->position = glm::vec2(8.f, 8.f);
        cars_list_table_view->set_background_color(glm::u8vec4(0, 0, 0, 32));
        cars_list_table_view->remove_component(gb::ces_bound_touch_component::class_guid());
        cars_list_dialog->add_control(cars_list_table_view, game::ces_ui_interaction_component::k_cars_list_dialog_table);
        cars_list_dialog->visible = true;
        
        return cars_list_dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_tutorial_steer_left_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(96.f, 192.f), "PRESS");
        label->position = glm::vec2(k_tutorial_steer_left_label_position);
        label->set_font_color(glm::u8vec4(255, 255, 255, 255));
        label->set_visible_edges(false);
        label->set_font_mode(gb::ces_font_component::e_font_mode_edge);
        label->remove_component(gb::ces_bound_touch_component::class_guid());
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_level_tutorial_steer_left_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_tutorial_steer_right_label(const std::string& filename)
    {
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(96.f, 192.f), "PRESS");
        label->position = glm::vec2(k_tutorial_steer_right_label_position);
        label->set_font_color(glm::u8vec4(255, 255, 255, 255));
        label->set_visible_edges(false);
        label->set_font_mode(gb::ces_font_component::e_font_mode_edge);
        label->remove_component(gb::ces_bound_touch_component::class_guid());
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_level_tutorial_steer_right_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_goto_racing_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_next.png", nullptr);
        button->position = k_goto_racing_button_position;
        button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_goto_racing_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_in_game_pause_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_pause.png", nullptr);
        button->position = k_pause_button_position;
        button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_pause_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_in_game_pause_menu_dialog(const std::string& filename)
    {
        auto pause_menu_dialog = gb::ces_entity::construct<gb::ui::dialog>();
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_pause_menu_dialog);
        pause_menu_dialog->add_component(ui_interaction_component);
        
        f32 x_offset = 8.f;
        const auto continue_button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_next.png", nullptr);
        continue_button->position = glm::vec2(x_offset, 8.f);
        continue_button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        continue_button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        continue_button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        x_offset += 16.f + 48.f;
        
        const auto restart_button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_restart.png", nullptr);
        restart_button->position = glm::vec2(x_offset, 8.f);
        restart_button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        restart_button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        restart_button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        x_offset += 16.f + 48.f;
        
        const auto exit_button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_exit.png", nullptr);
        exit_button->position = glm::vec2(x_offset, 8.f);
        exit_button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        exit_button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        exit_button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        x_offset += 8.f + 48.f;
        
        pause_menu_dialog->add_control(continue_button, game::ces_ui_interaction_component::k_pause_menu_dialog_continue_button);
        pause_menu_dialog->add_control(restart_button, game::ces_ui_interaction_component::k_pause_menu_dialog_restart_button);
        pause_menu_dialog->add_control(exit_button, game::ces_ui_interaction_component::k_pause_menu_dialog_exit_button);
        pause_menu_dialog->visible = false;
        pause_menu_dialog->position = glm::vec2(m_screen_size.x * .5f - x_offset * .5f, m_screen_size.y * .5 - 48.f);
        
        return pause_menu_dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_in_game_restart_dialog(const std::string& filename)
    {
        auto restart_dialog = gb::ces_entity::construct<gb::ui::dialog>();
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_restart_dialog);
        restart_dialog->add_component(ui_interaction_component);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(300.f, 128.f), "ARE YOU SURE ? IF YOU RESTART THE LEVEL YOU WILL MISS YOUR CURRENT PROGRESS AND ONE TICKET.");
        
        label->position = glm::vec2(-60.f, -24.f);
        label->set_font_color(glm::u8vec4(255, 255, 255, 255));
        label->set_background_color(glm::u8vec4(0, 0, 0, 32));
        label->set_foreground_color(glm::u8vec4(96, 96, 96, 96));
        label->set_font_size(20.f);
        label->set_multiline(true);
        restart_dialog->add_child(label);
        
        f32 x_offset = 8.f;
        const auto yes_button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_checkmark.png", nullptr);
        yes_button->position = glm::vec2(x_offset, 80.f);
        yes_button->set_image_color(glm::u8vec4(64, 64, 255, 255));
        yes_button->set_background_color(glm::u8vec4(0, 96, 0, 96));
        yes_button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        x_offset += 64.f + 48.f;
        
        const auto no_button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_cross.png", nullptr);
        no_button->position = glm::vec2(x_offset, 80.f);
        no_button->set_image_color(glm::u8vec4(255, 64, 64, 255));
        no_button->set_background_color(glm::u8vec4(96, 0, 0, 96));
        no_button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        x_offset += 8.f + 48.f;
        
        restart_dialog->add_control(yes_button, game::ces_ui_interaction_component::k_restart_dialog_yes_button);
        restart_dialog->add_control(no_button, game::ces_ui_interaction_component::k_restart_dialog_no_button);
        restart_dialog->visible = false;
        restart_dialog->position = glm::vec2(m_screen_size.x * .5f - x_offset * .5f, m_screen_size.y * .5 - 48.f);
        
        return restart_dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_in_game_quit_dialog(const std::string& filename)
    {
        auto quit_dialog = gb::ces_entity::construct<gb::ui::dialog>();
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_quit_dialog);
        quit_dialog->add_component(ui_interaction_component);
        
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(300.f, 128.f), "ARE YOU SURE ? IF YOU QUIT FROM RACE YOU WILL MISS YOUR CURRENT PROGRESS AND ONE TICKET.");
        
        label->position = glm::vec2(-60.f, -24.f);
        label->set_font_color(glm::u8vec4(255, 255, 255, 255));
        label->set_background_color(glm::u8vec4(0, 0, 0, 32));
        label->set_foreground_color(glm::u8vec4(96, 96, 96, 96));
        label->set_font_size(20.f);
        label->set_multiline(true);
        quit_dialog->add_child(label);
        
        f32 x_offset = 8.f;
        const auto yes_button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_checkmark.png", nullptr);
        yes_button->position = glm::vec2(x_offset, 80.f);
        yes_button->set_image_color(glm::u8vec4(64, 64, 255, 255));
        yes_button->set_background_color(glm::u8vec4(0, 96, 0, 96));
        yes_button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        x_offset += 64.f + 48.f;
        
        const auto no_button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_cross.png", nullptr);
        no_button->position = glm::vec2(x_offset, 80.f);
        no_button->set_image_color(glm::u8vec4(255, 64, 64, 255));
        no_button->set_background_color(glm::u8vec4(96, 0, 0, 96));
        no_button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        x_offset += 8.f + 48.f;
        
        quit_dialog->add_control(yes_button, game::ces_ui_interaction_component::k_quit_dialog_yes_button);
        quit_dialog->add_control(no_button, game::ces_ui_interaction_component::k_quit_dialog_no_button);
        quit_dialog->visible = false;
        quit_dialog->position = glm::vec2(m_screen_size.x * .5f - x_offset * .5f, m_screen_size.y * .5 - 48.f);
        
        return quit_dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_tickets_label(const std::string &filename)
    {
        const auto tickets_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(210.f, 24.f), "TICKETS: 3");
        tickets_label->position = k_tickets_label_position;
        tickets_label->set_font_color(glm::u8vec4(255, 255, 255, 255));
        tickets_label->set_foreground_color(glm::u8vec4(96, 96, 96, 96));
        tickets_label->set_background_color(glm::u8vec4(0, 0, 0, 32));
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_tickets_label);
        tickets_label->add_component(ui_interaction_component);
        
        const auto plus_ticket_button =  m_ui_base_fabricator.lock()->create_button(glm::vec2(32.f, 24.f), [=](gb::ces_entity_const_shared_ptr entity) {
            advertisement_provider::shared_instance()->play_rewarded_video();
        });
        plus_ticket_button->position = glm::vec2(178.f, 0.f);
        plus_ticket_button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        plus_ticket_button->set_text("+");
        plus_ticket_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        tickets_label->add_child(plus_ticket_button);
        
        return tickets_label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_end_game_dialog(const std::string& filename)
    {
        auto end_game_dialog = gb::ces_entity::construct<gb::ui::dialog>();
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_end_game_dialog);
        end_game_dialog->add_component(ui_interaction_component);
        
        const auto place_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(320.f, 24.f), "FINISHED AT PLACE: SECOND");
        
        place_label->position = glm::vec2(-80.f, -64.f);
        place_label->set_font_color(glm::u8vec4(255, 255, 255, 255));
        place_label->set_background_color(glm::u8vec4(0, 0, 0, 32));
        place_label->set_foreground_color(glm::u8vec4(96, 96, 96, 96));
        place_label->set_visible_edges(false);
        place_label->set_font_size(20.f);
        place_label->set_background_color(glm::u8vec4(96, 96, 96, 96));
        
        const auto drift_time_label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(320.f, 24.f), "DRIFT TIME: 19:45 sec");
        
        drift_time_label->position = glm::vec2(-80.f, -32.f);
        drift_time_label->set_font_color(glm::u8vec4(255, 255, 255, 255));
        drift_time_label->set_background_color(glm::u8vec4(0, 0, 0, 32));
        drift_time_label->set_foreground_color(glm::u8vec4(96, 96, 96, 96));
        drift_time_label->set_visible_edges(false);
        drift_time_label->set_font_size(20.f);
        drift_time_label->set_background_color(glm::u8vec4(96, 96, 96, 96));
        
        const auto star1_image = m_general_fabricator.lock()->create_sprite("ui_image.xml", "ui_star.png");
        star1_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
        star1_image->size = glm::vec2(64.f, 64.f);
        star1_image->color = glm::u8vec4(192, 0, 192, 255);
        star1_image->position = glm::vec2(-16.f, -4.f);
        
        const auto star2_image = m_general_fabricator.lock()->create_sprite("ui_image.xml", "ui_star.png");
        star2_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
        star2_image->size = glm::vec2(64.f, 64.f);
        star2_image->color = glm::u8vec4(192, 0, 192, 255);
        star2_image->position = glm::vec2(48.f, -4.f);
        
        const auto star3_image = m_general_fabricator.lock()->create_sprite("ui_image.xml", "ui_star.png");
        star3_image->get_component<gb::ces_transformation_component>()->set_is_in_camera_space(false);
        star3_image->size = glm::vec2(64.f, 64.f);
        star3_image->color = glm::u8vec4(32, 32, 32, 255);
        star3_image->position = glm::vec2(112.f, -4.f);
        
        f32 x_offset = 8.f;
        const auto continue_button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_next.png", nullptr);
        continue_button->position = glm::vec2(x_offset, 64.f);
        continue_button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        continue_button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        continue_button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        x_offset += 16.f + 48.f;
        
        const auto restart_button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_restart.png", nullptr);
        restart_button->position = glm::vec2(x_offset, 64.f);
        restart_button->set_image_color(glm::u8vec4(255, 255, 255, 255));
        restart_button->set_background_color(glm::u8vec4(96, 96, 96, 96));
        restart_button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        x_offset += 8.f + 48.f;
        
        end_game_dialog->add_control(place_label, game::ces_ui_interaction_component::k_end_game_dialog_place_label);
        end_game_dialog->add_control(drift_time_label, game::ces_ui_interaction_component::k_end_game_dialog_drift_time_label);
        end_game_dialog->add_control(star1_image, game::ces_ui_interaction_component::k_end_game_dialog_star1_image);
        end_game_dialog->add_control(star2_image, game::ces_ui_interaction_component::k_end_game_dialog_star2_image);
        end_game_dialog->add_control(star3_image, game::ces_ui_interaction_component::k_end_game_dialog_star3_image);
        end_game_dialog->add_control(continue_button, game::ces_ui_interaction_component::k_end_game_dialog_continue_button);
        end_game_dialog->add_control(restart_button, game::ces_ui_interaction_component::k_end_game_dialog_restart_button);
        
        end_game_dialog->visible = false;
        end_game_dialog->position = glm::vec2(m_screen_size.x * .5f - x_offset * .5f, m_screen_size.y * .5 - 48.f);
        
        return end_game_dialog;
    }
}
