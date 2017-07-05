//
//  ai_character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 3/9/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ai_character_controller.h"
#include "ces_action_component.h"
#include "ces_ai_component.h"
#include "character.h"
#include "animated_sprite.h"
#include "ai_move_action.h"
#include "ai_attack_action.h"
#include "ai_chase_action.h"
#include "ai_actions_processor.h"
#include "glm_extensions.h"
#include "ces_box2d_body_component.h"
#include "ces_character_controller_component.h"
#include "ces_character_statistic_component.h"
#include "hit_bounds.h"
#include "scene_graph.h"

#if !defined(__NO_RENDER__)

#include "information_bubble_controller.h"
#include "bloodprint_controller.h"
#include "footprint_controller.h"

#endif

#define k_footprint_timeinterval 333.f

namespace game
{
    ai_character_controller::ai_character_controller(const gb::scene_graph_shared_ptr& scene_graph,
                                                     const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                                     const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                                     const std::array<gb::game_object_2d_weak_ptr, level::e_level_layer_max>& layers) :
    game::client_base_character_controller(scene_graph, scene_fabricator, anim_fabricator, layers)
    {
        ces_entity::add_deferred_component_constructor<ces_ai_component>();
    }
    
    ai_character_controller::~ai_character_controller()
    {
        
    }
    
    void ai_character_controller::setup_components()
    {
        client_base_character_controller::setup_components();
        auto character_controller_component = ces_entity::get_component<ces_character_controller_component>();
        character_controller_component->mode = ces_character_controller_component::e_mode::ai;
        
        auto character_statistic_component = ces_entity::get_component<ces_character_statistic_component>();
        character_statistic_component->setup(25.f, 750.f, 2000.f, 5.f, 64.f);
    }
    
    void ai_character_controller::setup(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage)
    {
        client_base_character_controller::setup(character_linkage);
        
        std::static_pointer_cast<character>(m_character)->set_animation_end_callback(std::bind(&ai_character_controller::on_animation_ended,
                                                                                               this,
                                                                                               std::placeholders::_1,
                                                                                               std::placeholders::_2));
    }
    
    void ai_character_controller::on_attack()
    {
        std::static_pointer_cast<character>(m_character)->play_animation(character::animations::k_attack_animation, false);
    }
    
    void ai_character_controller::on_died(const gb::ces_entity_shared_ptr& entity)
    {
        client_base_character_controller::on_died(entity);
        auto ai_component = ai_character_controller::get_component<ces_ai_component>();
        ai_actions_processor_shared_ptr actions_processor = ai_component->actions_processor;
        actions_processor->interrupt_all_actions();
    }

    void ai_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        auto character_statistic_component = ai_character_controller::get_component<ces_character_statistic_component>();
        if(!character_statistic_component->is_dead)
        {
            glm::vec2 current_position = client_base_character_controller::position;
            m_character_statistic->position = current_position;
            
            auto ai_component = ai_character_controller::get_component<ces_ai_component>();
            ai_actions_processor_shared_ptr actions_processor = ai_component->actions_processor;
            
            auto action = actions_processor->top_action();
            if(action)
            {
                if(action->instance_guid() == ai_move_action::class_guid() ||
                   action->instance_guid() == ai_chase_action::class_guid())
                {
                    if(!action->is_progress_callback_exist())
                    {
                        action->set_in_progress_callback([this](const ai_action_shared_ptr& action) {
                            
                            if(action->instance_guid() == ai_move_action::class_guid() ||
                               action->instance_guid() == ai_chase_action::class_guid())
                            {
                                client_base_character_controller::on_move();
                                
#if !defined(__NO_RENDER__)
                                
                                std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
                                f32 deltatime = std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - m_footprint_previous_timestamp).count();
                                if(deltatime > k_footprint_timeinterval)
                                {
                                    m_footprint_previous_timestamp = current_timestamp;
                                    glm::vec2 current_position = ai_character_controller::position;
                                    f32 current_rotation = ai_character_controller::rotation;
                                    m_footprint_controller.lock()->push_footprint(glm::u8vec4(255, 255, 255, 255), current_position, current_rotation);
                                }
                                
#endif
                                
                            }
                        });
                    }
                }
                else if(action->instance_guid() == ai_attack_action::class_guid())
                {
                    if(!action->is_progress_callback_exist())
                    {
                        action->set_in_progress_callback([this](const ai_action_shared_ptr& action) {
                            
                            if(action->instance_guid() == ai_attack_action::class_guid())
                            {
                                ai_character_controller::on_attack();
                            }
                        });
                    }
                }
            }
            
            client_base_character_controller::on_health_updated();
        }
        else
        {
            std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
            f32 deltatime = std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - m_dead_timestamp).count();
            if(deltatime > m_dead_cooldown_timeinterval)
            {
                client_base_character_controller::on_revive();
            }
        }
    }
}

