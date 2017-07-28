//
//  ces_state_automat_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/25/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_state_automat_system.h"
#include "ces_character_state_automat_component.h"
#include "ces_character_animation_component.h"
#include "ces_character_controllers_component.h"
#include "ces_box2d_body_component.h"
#include "ai_actions_processor.h"
#include "footprint_controller.h"
#include "game_object_2d.h"
#include "ai_attack_action.h"
#include "ces_box2d_body_component.h"
#include "ces_character_statistic_component.h"
#include "sprite.h"
#include "ces_character_parts_component.h"

namespace game
{
    ces_state_automat_system::ces_state_automat_system()
    {
        ces_base_system::add_required_component_guid(m_state_automat_components_mask, ces_character_state_automat_component::class_guid());
        ces_base_system::add_required_components_mask(m_state_automat_components_mask);
    }
    
    ces_state_automat_system::~ces_state_automat_system()
    {
        
    }
    
    void ces_state_automat_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_state_automat_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_state_automat_components_mask, [dt](const gb::ces_entity_shared_ptr& entity) {
            
            std::string character_key = entity->tag;
            auto character_state_automat_component = entity->get_component<ces_character_state_automat_component>();
            auto actions_processor = character_state_automat_component->get_actions_processor();
            actions_processor->update(dt);
            
            switch (character_state_automat_component->get_state()) {
                case ces_character_state_automat_component::e_state_idle:
                {
                    auto box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
                    if(box2d_body_component)
                    {
                        box2d_body_component->velocity = glm::vec2(0.f);
                    }
                    
                    auto character_animation_component = entity->get_component<ces_character_animation_component>();
                    character_animation_component->play_animation(ces_character_animation_component::animations::k_idle_animation, true);
                }
                    break;
                case ces_character_state_automat_component::e_state_move:
                case ces_character_state_automat_component::e_state_chase:
                {
                    auto character_animation_component = entity->get_component<ces_character_animation_component>();
                    character_animation_component->play_animation(ces_character_animation_component::animations::k_walk_animation, true);
                    auto character_controllers_component = entity->get_component<ces_character_controllers_component>();
                    footprint_controller_shared_ptr footprint_controller = character_controllers_component->footprint_controller;
                    if(footprint_controller->is_ready_to_push_footprint())
                    {
                        footprint_controller->push_footprint(glm::u8vec4(255, 255, 255, 255),
                                                             std::static_pointer_cast<gb::game_object_2d>(entity)->position,
                                                             std::static_pointer_cast<gb::game_object_2d>(entity)->rotation);
                    }
                }
                    break;
                case ces_character_state_automat_component::e_state_attack:
                {
                    auto box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
                    if(box2d_body_component)
                    {
                        box2d_body_component->velocity = glm::vec2(0.f);
                    }
                    
                    auto attack_action = actions_processor->top_action();
                    if(attack_action->instance_guid() == ai_attack_action::class_guid())
                    {
                        attack_action->set_in_progress_callback([](const ai_action_shared_ptr& action) {
                            auto character = action->get_owner();
                            auto character_animation_component = character->get_component<ces_character_animation_component>();
                            character_animation_component->play_animation(ces_character_animation_component::animations::k_attack_animation, false);
                        });
                    }
                    else
                    {
                        assert(false);
                    }
                }
                    break;
                case ces_character_state_automat_component::e_state_die:
                {
                    auto character_animation_component = entity->get_component<ces_character_animation_component>();
                    character_animation_component->play_animation(ces_character_animation_component::animations::k_die_animation, false);
                    character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_dead);
                    auto character_statistic_component = entity->get_component<ces_character_statistic_component>();
                    character_statistic_component->set_dead_timestamp(std::chrono::steady_clock::now());
                }
                    break;
                case ces_character_state_automat_component::e_state_dead:
                {
                    auto character_statistic_component = entity->get_component<ces_character_statistic_component>();
                    std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
                    f32 dt = std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - character_statistic_component->get_dead_timestamp()).count();
                    if(dt > character_statistic_component->current_reviving_time)
                    {
                        auto character_parts_component = entity->get_component<ces_character_parts_component>();
                        std::static_pointer_cast<gb::sprite>(character_parts_component->get_body_part())->color = glm::u8vec4(255);
                        auto box2d_body_component = entity->get_component<gb::ces_box2d_body_component>();
                        box2d_body_component->enabled = true;
                        character_statistic_component->reset();
                        character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_idle);
                    }
                }
                    break;
                default:
                    break;
            }
        });
    }
    
    void ces_state_automat_system::on_feed_end(f32 dt)
    {
        
    }
}
