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
#include "image_button.h"
#include "joystick.h"
#include "textfield.h"
#include "table_view.h"
#include "action_console.h"

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
    
    gameplay_ui_fabricator::gameplay_ui_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator,
                                                   const gb::ui::ui_fabricator_shared_ptr& ui_base_fabricator,
                                                   const glm::ivec2& screen_size) :
    m_general_fabricator(general_fabricator),
    m_ui_base_fabricator(ui_base_fabricator),
    m_screen_size(screen_size)
    {
        k_next_car_in_garage_button_position.x = m_screen_size.x - 64.f - 8.f;
        k_next_car_in_garage_button_position.y = m_screen_size.y - 64.f - 8.f;
        k_prev_car_in_garage_button_position.x = 8.f;
        k_prev_car_in_garage_button_position.y = m_screen_size.y - 64.f - 8.f;
        
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
    }
    
    glm::ivec2 gameplay_ui_fabricator::get_screen_size() const
    {
        return m_screen_size;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_open_levels_list_dialog_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(48.f, 48.f), "ui_trophy.png", nullptr);
        button->position = k_open_levels_list_dialog_button_position;
        button->set_image_color(glm::u8vec4(32, 32, 32, 255));
        button->set_background_color(glm::u8vec4(192, 192, 192, 192));
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
        button->set_image_color(glm::u8vec4(32, 32, 32, 255));
        button->set_background_color(glm::u8vec4(192, 192, 192, 192));
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
        button->set_image_color(glm::u8vec4(32, 32, 32, 255));
        button->set_background_color(glm::u8vec4(192, 192, 192, 192));
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
        levels_list_table_view->set_background_color(glm::u8vec4(192, 192, 192, 192));
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
        const auto label = m_ui_base_fabricator.lock()->create_textfield(glm::vec2(220.f, 24.f), "TIME: 00:00 sec");
        label->position = glm::vec2(m_screen_size.x * .5f - 220.f * .5f,
                                    8.f);
        label->set_font_color(glm::u8vec4(255, 255, 0, 255));
        label->set_visible_edges(false);
        label->visible = false;
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_countdown_label);
        label->add_component(ui_interaction_component);
        
        return label;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_next_car_in_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(64.f, 64.f), "ui_prev.png", nullptr);
        button->position = k_next_car_in_garage_button_position;
        button->set_image_color(glm::u8vec4(32, 32, 32, 128));
        button->set_background_color(glm::u8vec4(192, 192, 192, 64));
        button->attach_sound("button_press.mp3", gb::ui::button::k_pressed_state);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_ui(game::ces_ui_interaction_component::e_ui_prev_car_in_garage_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_prev_car_in_garage_button(const std::string& filename)
    {
        const auto button = m_ui_base_fabricator.lock()->create_image_button(glm::vec2(64.f, 64.f), "ui_next.png", nullptr);
        button->position = k_prev_car_in_garage_button_position;
        button->set_image_color(glm::u8vec4(32, 32, 32, 128));
        button->set_background_color(glm::u8vec4(192, 192, 192, 64));
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
        button->set_image_color(glm::u8vec4(32, 32, 32, 128));
        button->set_background_color(glm::u8vec4(192, 192, 192, 64));
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
        button->set_image_color(glm::u8vec4(32, 32, 32, 128));
        button->set_background_color(glm::u8vec4(192, 192, 192, 64));
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
        button->set_image_color(glm::u8vec4(32, 32, 32, 128));
        button->set_background_color(glm::u8vec4(192, 192, 192, 64));
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
                                                                                             192.f));
        cars_list_table_view->position = glm::vec2(m_screen_size.x - 162.f, 40.f);
        cars_list_table_view->set_background_color(glm::u8vec4(192, 192, 192, 64));
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
}
