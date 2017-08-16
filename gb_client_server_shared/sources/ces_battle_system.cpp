//
//  ces_battle_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_battle_system.h"
#include "ces_hit_bounds_component.h"
#include "ces_box2d_body_component.h"
#include "ces_character_controllers_component.h"
#include "ces_character_statistic_component.h"
#include "ces_character_animation_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_character_state_automat_component.h"
#include "ces_character_selector_component.h"
#include "hit_bounds_controller.h"
#include "information_bubble_controller.h"
#include "bloodprint_controller.h"
#include "std_extensions.h"
#include "sprite.h"
#include "ai_actions_processor.h"
#include "ces_character_blinking_effect_component.h"

namespace game
{
    ces_battle_system::ces_battle_system()
    {
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_controllers_component::class_guid());
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_animation_component::class_guid());
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_statistic_component::class_guid());
        ces_base_system::add_required_components_mask(m_character_components_mask);
        
        ces_base_system::add_required_component_guid(m_hit_bounds_components_mask, ces_hit_bounds_component::class_guid());
        ces_base_system::add_required_components_mask(m_hit_bounds_components_mask);
    }
    
    ces_battle_system::~ces_battle_system()
    {
        
    }
    
    void ces_battle_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_battle_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_character_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            auto character_animation_component = entity->get_component<ces_character_animation_component>();
            if(!character_animation_component->is_animation_ended_callback_exist(entity))
            {
                character_animation_component->add_on_amimation_ended_callback(entity, [] (const gb::ces_entity_shared_ptr& entity, const std::string& animation_name, bool is_looped) {
                    if(animation_name == ces_character_animation_component::animations::k_attack_animation)
                    {
                        auto transformation_2d_component = entity->get_component<gb::ces_transformation_2d_component>();
                        auto character_controllers_component = entity->get_component<ces_character_controllers_component>();
                        hit_bounds_controller_shared_ptr hit_bounds_controller = character_controllers_component->hit_bounds_controller;
                        hit_bounds_controller->push_hit_bounds(entity, transformation_2d_component->get_position(), transformation_2d_component->get_rotation());
                    }
                    if(animation_name == ces_character_animation_component::animations::k_die_animation)
                    {
                        auto character_blinking_effect_component = std::make_shared<ces_character_blinking_effect_component>();
                        character_blinking_effect_component->set_blinking_effect_ended_callback([](const gb::ces_entity_shared_ptr& entity) {
                            auto character_animation_component = entity->get_component<ces_character_animation_component>();
                            character_animation_component->play_animation(ces_character_animation_component::animations::k_idle_animation, true);
                            auto character_statistic_component = entity->get_component<ces_character_statistic_component>();
                            std::static_pointer_cast<gb::game_object_2d>(entity)->position = character_statistic_component->get_spawn_position();
                        });
                        entity->add_component(character_blinking_effect_component);
                    }
                });
            }
            auto character_statistic_component = entity->get_component<ces_character_statistic_component>();
            auto character_transformation_component = entity->get_component<gb::ces_transformation_2d_component>();
            character_statistic_component->get_health_status_entity()->position = character_transformation_component->get_position();
            if(!character_statistic_component->is_dead)
            {
                f32 current_health_percents = character_statistic_component->current_health_percents;
                character_statistic_component->get_health_status_entity()->color = glm::mix(glm::u8vec4(255, 0, 0, 255), glm::u8vec4(0, 255, 0, 255), current_health_percents);
            }
            else
            {
                character_statistic_component->get_health_status_entity()->color = glm::u8vec4(0);
            }
        });
        
        ces_base_system::enumerate_entities_with_components(m_hit_bounds_components_mask, [](const gb::ces_entity_shared_ptr& entity) {
            auto hit_bounds_component = entity->get_component<ces_hit_bounds_component>();
            if(!hit_bounds_component->is_hit_callback_exist())
            {
                hit_bounds_component->set_hit_callback([] (const gb::ces_entity_shared_ptr& entity) {
                    auto hit_bounds_component = entity->get_component<ces_hit_bounds_component>();
                    auto current_character = hit_bounds_component->get_executor();
                    auto box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
                    gb::ces_entity_shared_ptr opponent_character = box2d_body_component->contacted_entity;
                    auto opponent_character_box2d_body_component = opponent_character->get_component<gb::ces_box2d_body_component>();
                    opponent_character_box2d_body_component->contacted_entity = nullptr;
                    
                    auto opponent_character_statistic_component = opponent_character->get_component<ces_character_statistic_component>();
                    if(opponent_character_statistic_component)
                    {
                        auto opponent_character_transformation_component = opponent_character->get_component<gb::ces_transformation_2d_component>();
                        auto current_character_statistic_component = current_character->get_component<ces_character_statistic_component>();
                        auto current_character_controller_component = current_character->get_component<ces_character_controllers_component>();
                        auto current_character_transformation_component = current_character->get_component<gb::ces_transformation_2d_component>();
                        
                        f32 current_character_damage = current_character_statistic_component->current_damage;
                        f32 opponent_character_health = opponent_character_statistic_component->current_health;
                        opponent_character_health -= current_character_damage;
                        opponent_character_statistic_component->current_health = opponent_character_health;
                        
                        if(opponent_character_statistic_component->is_dead)
                        {
                            const auto& character_state_automat_component = opponent_character->get_component<ces_character_state_automat_component>();
                            character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_die);
                            auto actions_processor = character_state_automat_component->get_actions_processor();
                            actions_processor->interrupt_all_actions();
                            auto box2d_body_component = opponent_character->get_component<gb::ces_box2d_body_component>();
                            box2d_body_component->enabled = false;
                            auto opponent_character_selector_component = opponent_character->get_component<ces_character_selector_component>();
                            opponent_character_selector_component->remove_all_selections();
                            auto current_character_selector_component = current_character->get_component<ces_character_selector_component>();
                            current_character_selector_component->remove_selection(opponent_character);
                        }
                        
                        std::stringstream string_stream;
                        string_stream<<-current_character_damage;
                        information_bubble_controller_shared_ptr information_bubble_controller = current_character_controller_component->information_bubble_controller;
                        information_bubble_controller->push_bubble(string_stream.str(),  glm::u8vec4(255, 0, 0, 255),
                                                                   opponent_character_transformation_component->get_position(),
                                                                   opponent_character_transformation_component->get_rotation(), 2);
                        
                        glm::vec2 direction = glm::normalize(opponent_character_transformation_component->get_position() - current_character_transformation_component->get_position());
                        bloodprint_controller_shared_ptr bloodprint_controller = current_character_controller_component->bloodprint_controller;
                        bloodprint_controller->push_bloodprint(glm::u8vec4(255, 0, 0, 255), opponent_character_transformation_component->get_position() + direction * std::get_random_f(32.f, 48.f),
                                                               opponent_character_transformation_component->get_rotation());
                    }
                });
            }
        });
    }
    
    void ces_battle_system::on_feed_end(f32 dt)
    {
        
    }
}

