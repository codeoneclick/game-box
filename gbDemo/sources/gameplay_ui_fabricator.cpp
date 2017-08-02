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
#include "dialog.h"
#include "button.h"

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
    
    gb::game_object_2d_shared_ptr gameplay_ui_fabricator::create_quest_dialog(const std::string& filename, const glm::ivec2& screen_size)
    {
        auto quest_accept_button = m_ui_fabricator.lock()->create_button(glm::vec2(128.f, 32.f), nullptr);
        quest_accept_button->position = glm::vec2(64.f, screen_size.y * .75f);
        quest_accept_button->set_text("accept");
        quest_accept_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        
        auto quest_decline_button = m_ui_fabricator.lock()->create_button(glm::vec2(128.f, 32.f), nullptr);
        quest_decline_button->position = glm::vec2(screen_size.x - 64.f - 128.f, screen_size.y * .75f);
        quest_decline_button->set_text("decline");
        quest_decline_button->attach_sound("sound_01.mp3", gb::ui::button::k_pressed_state);
        
        auto quest_dialog = gb::ces_entity::construct<gb::ui::dialog>();
        quest_dialog->add_control(quest_accept_button, 1);
        quest_dialog->add_control(quest_decline_button, 2);
        
        auto ui_interaction_component = std::make_shared<ces_ui_interaction_component>();
        ui_interaction_component->set_type(game::ces_ui_interaction_component::e_type_quest_dialog);
        quest_dialog->add_component(ui_interaction_component);

        return quest_dialog;
    }
}
