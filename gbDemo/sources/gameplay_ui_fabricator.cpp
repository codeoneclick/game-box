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
#include "ces_bound_touch_component.h"
#include "ces_ui_avatar_icon_component.h"
#include "ces_ui_quest_dialog_component.h"
#include "ces_ui_questlog_dialog_component.h"
#include "dialog.h"
#include "button.h"
#include "textfield.h"
#include "table_view.h"
#include "action_console.h"

namespace game
{
    gameplay_ui_fabricator::gameplay_ui_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator,
                                                   const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                                   const gb::ui::ui_fabricator_shared_ptr& ui_fabricator) :
    m_general_fabricator(general_fabricator),
    m_anim_fabricator(anim_fabricator),
    m_ui_fabricator(ui_fabricator)
    {
        m_gameplay_configuration_accessor = std::make_shared<gb::gameplay_configuration_accessor>();
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_attack_button(const std::string& filename, const glm::ivec2& screen_size)
    {
        gb::game_object_2d_shared_ptr button = gb::ces_entity::construct<gb::game_object_2d>();
        button->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
        auto background = m_general_fabricator.lock()->create_sprite("ability.button.xml");
        background->size = glm::vec2(64.f);
        background->color = glm::u8vec4(255, 255, 0, 255);
        background->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
        button->add_child(background);
        
        auto bound_touch_component = std::make_shared<gb::ces_bound_touch_component>();
        bound_touch_component->set_bounds(glm::vec4(0.f, 0.f,
                                                    64.f, 64.f));
        button->add_component(bound_touch_component);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_attack_button);
        button->add_component(ui_interaction_component);
        button->position = glm::vec2(16.f, screen_size.y - 80.f);
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_abitily_button(const std::string& filename, const glm::ivec2& screen_size)
    {
        gb::game_object_2d_shared_ptr button = gb::ces_entity::construct<gb::game_object_2d>();
        auto background = m_general_fabricator.lock()->create_sprite("ability.button.xml");
        background->size = glm::vec2(48.f);
        background->color = glm::u8vec4(0, 255, 255, 255);
        background->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
        button->add_child(background);
        
        auto bound_touch_component = std::make_shared<gb::ces_bound_touch_component>();
        bound_touch_component->set_bounds(glm::vec4(0.f, 0.f,
                                                    48.f, 48.f));
        button->add_component(bound_touch_component);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_ability_button);
        button->add_component(ui_interaction_component);
        
        return button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_character_avatar_icon(const std::string& filename, const glm::ivec2& screen_size)
    {
        gb::game_object_2d_shared_ptr icon = gb::ces_entity::construct<gb::game_object_2d>();
        auto background = m_general_fabricator.lock()->create_sprite("ability.button.xml");
        background->size = glm::vec2(80.f);
        background->color = glm::u8vec4(255, 255, 255, 255);
        background->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
        icon->add_child(background);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_character_avatar_icon);
        icon->add_component(ui_interaction_component);
        
        auto ui_avatar_icon_component = std::make_shared<ces_ui_avatar_icon_component>();
        icon->add_component(ui_avatar_icon_component);
        icon->position = glm::vec2(8.f);
        return icon;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_opponent_avatar_icon(const std::string& filename, const glm::ivec2& screen_size)
    {
        gb::game_object_2d_shared_ptr icon = gb::ces_entity::construct<gb::game_object_2d>();
        auto background = m_general_fabricator.lock()->create_sprite("ability.button.xml");
        background->size = glm::vec2(80.f);
        background->color = glm::u8vec4(255, 255, 255, 255);
        background->get_component<gb::ces_transformation_2d_component>()->set_is_in_camera_space(false);
        icon->add_child(background);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_opponent_avatar_icon);
        icon->add_component(ui_interaction_component);
        
        auto ui_avatar_icon_component = std::make_shared<ces_ui_avatar_icon_component>();
        icon->add_component(ui_avatar_icon_component);
        icon->position = glm::vec2(screen_size.x - 88.f, 8.f);
        return icon;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_quests_dialog(const std::string& filename, const glm::ivec2& screen_size)
    {
        auto quests_dialog = gb::ces_entity::construct<gb::ui::dialog>();
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_quest_dialog);
        quests_dialog->add_component(ui_interaction_component);
        
        auto quests_table_view = m_ui_fabricator.lock()->create_table_view(glm::vec2(screen_size.x - screen_size.x * .33f - 42.f,
                                                                                     screen_size.y - 48.f));
        quests_table_view->position = glm::vec2(screen_size.x * .33f + 8.f, 40.f);
        quests_table_view->set_background_color(glm::u8vec4(128, 128, 128, 192));
        quests_dialog->add_control(quests_table_view, game::ces_ui_interaction_component::k_quests_dialog_quests_table);
        
        auto quest_title_textfield = m_ui_fabricator.lock()->create_textfield(glm::vec2(screen_size.x - 16.f, 32.f), "NPC Name");
        quest_title_textfield->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        quest_title_textfield->position = glm::vec2(8.f, 8.f);
        quests_dialog->add_control(quest_title_textfield, game::ces_ui_interaction_component::k_quests_dialog_title_label);
        
        auto biography_textfield = m_ui_fabricator.lock()->create_textfield(glm::vec2(screen_size.x * .33f, screen_size.y - 48.f), "Description");
        biography_textfield->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        biography_textfield->position = glm::vec2(8.f, 40.f);
        quests_dialog->add_control(biography_textfield, game::ces_ui_interaction_component::k_quests_dialog_biography_label);
        
        auto questlog_close_button = m_ui_fabricator.lock()->create_button(glm::vec2(32.f, 32.f), nullptr);
        questlog_close_button->position = glm::vec2(screen_size.x - 32.f - 8.f, 8.f);
        questlog_close_button->set_text("x");
        questlog_close_button->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        questlog_close_button->set_background_color(glm::u8vec4(255, 0, 0, 255));
        questlog_close_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        quests_dialog->add_control(questlog_close_button, game::ces_ui_interaction_component::k_quests_dialog_close_button);
        
        auto ui_quest_dialog_component = std::make_shared<ces_ui_quest_dialog_component>();
        quests_dialog->add_component(ui_quest_dialog_component);

        return quests_dialog;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_action_console(const std::string& filename, const glm::ivec2& screen_size)
    {
        auto action_console = m_ui_fabricator.lock()->create_action_console(glm::vec2(256.f, 72.f), 4);
        action_console->position = glm::vec2(screen_size.x * .5f - 128.f, 8.f);
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_action_console);
        action_console->add_component(ui_interaction_component);
        return action_console;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_questlog_button(const std::string& filename, const glm::ivec2& screen_size)
    {
        auto questlog_button = m_ui_fabricator.lock()->create_button(glm::vec2(128.f, 32.f), nullptr);
        questlog_button->position = glm::vec2(screen_size.x - 128.f - 8.f, screen_size.y - 32.f - 8.f);
        questlog_button->set_text("QuestLog");
        questlog_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_questlog_button);
        questlog_button->add_component(ui_interaction_component);
        return questlog_button;
    }
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_questlog_dialog(const std::string& filename, const glm::ivec2& screen_size)
    {
        auto questlog_dialog = gb::ces_entity::construct<gb::ui::dialog>();
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_questlog_dialog);
        questlog_dialog->add_component(ui_interaction_component);
        
        auto quests_table_view = m_ui_fabricator.lock()->create_table_view(glm::vec2(screen_size.x - 36.f,
                                                                                     screen_size.y - 16.f));
        quests_table_view->position = glm::vec2(8.f);
        quests_table_view->set_background_color(glm::u8vec4(128, 128, 128, 192));
        questlog_dialog->add_control(quests_table_view, game::ces_ui_interaction_component::k_questlog_dialog_quests_table);
        
        auto no_quests_textfield = m_ui_fabricator.lock()->create_textfield(glm::vec2(screen_size.x, 32.f), "No Quests");
        no_quests_textfield->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        no_quests_textfield->position = glm::vec2(0.f, screen_size.y * .5f - 32.f);
        questlog_dialog->add_control(no_quests_textfield, game::ces_ui_interaction_component::k_questlog_dialog_no_quests_label);
        
        auto questlog_close_button = m_ui_fabricator.lock()->create_button(glm::vec2(32.f, 32.f), nullptr);
        questlog_close_button->position = glm::vec2(screen_size.x - 32.f - 4.f, 4.f);
        questlog_close_button->set_text("x");
        questlog_close_button->set_text_horizontal_aligment(gb::ui::e_element_horizontal_aligment_center);
        questlog_close_button->set_background_color(glm::u8vec4(255, 0, 0, 255));
        questlog_close_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        questlog_dialog->add_control(questlog_close_button, game::ces_ui_interaction_component::k_questlog_dialog_close_button);
        
        return questlog_dialog;
    }
}
