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
#include "ai_attack_move_action.h"
#include "ai_actions_processor.h"
#include "glm_extensions.h"
#include "ces_box2d_body_component.h"
#include "ces_character_controller_component.h"
#include "bullet.h"
#include "scene_graph.h"
#include "information_bubble_controller.h"
#include "bloodprint_controller.h"
#include "footprint_controller.h"

#define k_footprint_timeinterval 333.f

namespace game
{
    ai_character_controller::ai_character_controller(const gb::scene_graph_shared_ptr& scene_graph,
                                                     const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                                     const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                                     const std::array<gb::game_object_weak_ptr, level::e_level_layer_max>& layers) :
    game::client_base_character_controller(scene_graph, scene_fabricator, anim_fabricator, layers)
    {
        ces_ai_component_shared_ptr ai_component = std::make_shared<ces_ai_component>();
        ai_character_controller::add_component(ai_component);
        
        auto character_controller_component = ai_character_controller::get_component<game::ces_character_controller_component>();
        character_controller_component->mode = ces_character_controller_component::e_mode::ai;
    }
    
    ai_character_controller::~ai_character_controller()
    {
        
    }
    
    void ai_character_controller::on_shoot()
    {
        bullet_shared_ptr bullet = std::make_shared<game::bullet>();
        bullet->setup("ns_bullet_01.xml",
                      m_scene_graph.lock(),
                      m_scene_fabricator.lock(),
                      m_anim_fabricator.lock());
        m_layers[level::e_level_layer_bullets].lock()->add_child(bullet);
        
        f32 current_rotation = ai_character_controller::rotation;
        current_rotation += 180.f;
        glm::vec2 current_position = ai_character_controller::position;
        current_position += glm::vec2(-sinf(glm::radians(current_rotation + 10.f)) * 64.f,
                                      cosf(glm::radians(current_rotation + 10.f)) * 64.f);
        
        
        m_scene_graph.lock()->apply_box2d_physics(bullet, b2BodyType::b2_dynamicBody, [](gb::ces_box2d_body_component_const_shared_ptr component) {
            component->shape = gb::ces_box2d_body_component::circle;
            component->set_radius(8.f);
        });
        
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        bullet->get_component<gb::ces_box2d_body_component>();
        box2d_body_component->is_destuctable_on_contact = true;
        
        b2Body *body = box2d_body_component->box2d_body;
        body->SetBullet(true);
        
        f32 current_move_speed = 100000.f;
        
        glm::vec2 velocity = glm::vec2(-sinf(glm::radians(current_rotation)) * current_move_speed,
                                       cosf(glm::radians(current_rotation)) * current_move_speed);
        bullet->position = current_position;
        bullet->rotation = current_rotation;
        box2d_body_component->velocity = velocity;
    }
    
    void ai_character_controller::on_spawn(const gb::ces_entity_shared_ptr& entity)
    {
        client_base_character_controller::on_spawn(entity);
        auto ai_component = ai_character_controller::get_component<ces_ai_component>();
        ai_actions_processor_shared_ptr actions_processor = ai_component->actions_processor;
        actions_processor->interrupt_all_actions();
    }
    
#define k_move_speed -1000.f
#define k_rotate_speed 100.f
    
    void ai_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 deltatime)
    {
        gb::ces_box2d_body_component_shared_ptr box2d_body_component =
        ai_character_controller::get_component<gb::ces_box2d_body_component>();
        
        auto ai_component = ai_character_controller::get_component<ces_ai_component>();
        ai_actions_processor_shared_ptr actions_processor = ai_component->actions_processor;
        
        auto action = actions_processor->top_action();
        if(action)
        {
            if(action->instance_guid() == ai_move_action::class_guid())
            {
                if(!action->is_progress_callback_exist())
                {
                    action->set_in_progress_callback([this](const ai_action_shared_ptr& action) {
                        
                        if(action->instance_guid() == ai_move_action::class_guid())
                        {
                            client_base_character_controller::on_move();
                            std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
                            f32 deltatime = std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - m_footprint_previous_timestamp).count();
                            if(deltatime > k_footprint_timeinterval)
                            {
                                m_footprint_previous_timestamp = current_timestamp;
                                glm::vec2 current_position = ai_character_controller::position;
                                f32 current_rotation = ai_character_controller::rotation;
                                m_footprint_controller.lock()->push_footprint(glm::u8vec4(255, 255, 255, 255), current_position, current_rotation);
                            }
                        }
                    });
                }
                if(!action->is_end_callback_exist())
                {
                    action->set_end_callback([this](const ai_action_shared_ptr& action) {
                        client_base_character_controller::on_idle();
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
                            ai_character_controller::on_shoot();
                            client_base_character_controller::on_idle();
                        }
                        else if(action->instance_guid() == ai_attack_move_action::class_guid())
                        {
                            client_base_character_controller::on_move();
                            std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
                            f32 deltatime = std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - m_footprint_previous_timestamp).count();
                            if(deltatime > k_footprint_timeinterval)
                            {
                                m_footprint_previous_timestamp = current_timestamp;
                                glm::vec2 current_position = ai_character_controller::position;
                                f32 current_rotation = ai_character_controller::rotation;
                                m_footprint_controller.lock()->push_footprint(glm::u8vec4(255, 255, 255, 255), current_position, current_rotation);
                            }
                        }
                    });
                }
            }
        }
    }
}

