//
//  client_main_character_controller.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "client_main_character_controller.h"
#include "button.h"
#include "ces_character_controller_component.h"
#include "ces_character_statistic_component.h"
#include "ces_box2d_body_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_character_battle_component.h"
#include "ces_light_mask_component.h"
#include "ces_geometry_component.h"
#include "game_object_2d.h"
#include "animated_sprite.h"
#include "camera_2d.h"
#include "character.h"
#include "scene_graph.h"
#include "glm_extensions.h"
#include "ces_transformation_extension.h"
#include "footprint_controller.h"
#include "information_bubble_controller.h"
#include "pathfinder.h"
#include "path_map.h"
#include "light_source_2d.h"
#include "ai_actions_processor.h"
#include "ai_move_action.h"
#include "ai_chase_action.h"
#include "ai_attack_action.h"

#define k_footprint_timeinterval 333.f

namespace game
{
    client_main_character_controller::client_main_character_controller(bool is_net_session,
                                                                       const gb::camera_2d_shared_ptr& camera,
                                                                       const gb::scene_graph_shared_ptr& scene_graph,
                                                                       const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                                                       const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                                                       const std::array<gb::game_object_2d_weak_ptr, level::e_level_layer_max>& layers) :
    game::client_base_character_controller(scene_graph, scene_fabricator, anim_fabricator, layers),
    m_is_net_session(is_net_session),
    m_camera(camera),
    m_map_size(0.f),
    m_on_death_cooldown_callback(nullptr),
    m_on_died_callback(nullptr),
    m_on_killed_callback(nullptr),
    m_path_map(nullptr),
    m_pathfinder(std::make_shared<pathfinder>()),
    m_actions_processor(std::make_shared<ai_actions_processor>()),
    m_is_locked_on_attack(false)
    {
       ces_entity::add_deferred_component_constructor<ces_character_battle_component>();
    }
    
    client_main_character_controller::~client_main_character_controller()
    {
        
    }
    
    void client_main_character_controller::setup_components()
    {
        client_base_character_controller::setup_components();
        auto character_controller_component = ces_entity::get_component<ces_character_controller_component>();
        character_controller_component->mode = ces_character_controller_component::e_mode::main;
        
        auto character_statistic_component = ces_entity::get_component<ces_character_statistic_component>();
        character_statistic_component->setup(100.f, 1000.f, 2000.f, 10.f, 64.f);
    }
    
#define k_shoot_speed 10000.f
    
    void client_main_character_controller::on_attack()
    {
        m_is_locked_on_attack = true;
        std::static_pointer_cast<character>(m_character)->play_animation(character::animations::k_attack_animation);
    }
    
    void client_main_character_controller::set_map_size(const glm::vec2& map_size)
    {
        m_map_size = map_size;
    }
    
    void client_main_character_controller::setup(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage)
    {
        client_base_character_controller::setup(character_linkage);
        std::static_pointer_cast<character>(m_character)->set_animation_end_callback(std::bind(&client_main_character_controller::on_animation_ended,
                                                                                               this,
                                                                                               std::placeholders::_1,
                                                                                               std::placeholders::_2));
        
        gb::light_source_2d_shared_ptr light_source_entity = std::static_pointer_cast<gb::light_source_2d>(m_character->get_child(character::parts::k_light_source_part, true));
        light_source_entity->color = glm::vec4(0.f, 1.f, 0.f, 1.f);
    }
    
    void client_main_character_controller::update(const gb::ces_entity_shared_ptr& entity, f32 dt)
    {
        m_actions_processor->update(dt);
        client_base_character_controller::on_health_updated();
        
        auto character_statistic_component = client_main_character_controller::get_component<ces_character_statistic_component>();
        if(!character_statistic_component->is_dead)
        {
            if(m_actions_processor->is_actions_exist())
            {
                
            }
            else
            {
                auto box2d_body_component = client_main_character_controller::get_component<gb::ces_box2d_body_component>();
                box2d_body_component->velocity = glm::vec2(0.f);
                if(!m_is_locked_on_attack)
                {
                    client_base_character_controller::on_idle();
                }
            }
            
            glm::vec2 current_position = client_base_character_controller::position;
            m_character_statistic->position = current_position;
            
            glm::vec2 camera_position = m_camera->get_position();
            camera_position = glm::mix(camera_position, current_position * -1.f, .5f);
            glm::ivec2 viewport_size = m_camera->viewport_size;
            glm::vec2 camera_pivot = m_camera->pivot;
            camera_position = glm::clamp(camera_position,
                                         glm::vec2(-m_map_size.x + static_cast<f32>(viewport_size.x) * camera_pivot.x,
                                                   -m_map_size.y + static_cast<f32>(viewport_size.y) * camera_pivot.y),
                                         glm::vec2(static_cast<f32>(viewport_size.x) * -camera_pivot.x,
                                                   static_cast<f32>(viewport_size.y) * -camera_pivot.y));
            m_camera->set_position(camera_position);
        }
        else
        {
            std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
            f32 deltatime = std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - m_dead_timestamp).count();
            if(deltatime < m_dead_cooldown_timeinterval)
            {
                if(m_on_death_cooldown_callback)
                {
                    m_on_death_cooldown_callback(static_cast<i32>((m_dead_cooldown_timeinterval - deltatime) / 1000.f), static_cast<i32>(m_dead_cooldown_timeinterval - deltatime) % 1000);
                }
            }
            else
            {
                if(m_on_death_cooldown_callback)
                {
                    m_on_death_cooldown_callback(0, 0);
                }
                client_base_character_controller::on_revive();
            }
        }
    }
    
    void client_main_character_controller::on_move_action_callback(const ai_action_shared_ptr& action)
    {
        client_base_character_controller::on_move();
        
#if !defined(__NO_RENDER__)
        
        std::chrono::steady_clock::time_point current_timestamp = std::chrono::steady_clock::now();
        f32 deltatime = std::chrono::duration_cast<std::chrono::milliseconds>(current_timestamp - m_footprint_previous_timestamp).count();
        if(deltatime > k_footprint_timeinterval)
        {
            glm::vec2 current_position = client_base_character_controller::position;
            f32 current_rotation = client_base_character_controller::rotation;
            
            m_footprint_previous_timestamp = current_timestamp;
            m_footprint_controller.lock()->push_footprint(glm::u8vec4(255, 255, 255, 255), current_position, current_rotation);
        }
        
#endif
        
    }
    
    void client_main_character_controller::on_attack_action_callback(const ai_action_shared_ptr& action)
    {
        client_main_character_controller::on_attack();
    }
    
    void client_main_character_controller::on_animation_ended(const std::string& animation_name, bool is_looped)
    {
        client_base_character_controller::on_animation_ended(animation_name, is_looped);
        if(animation_name == character::animations::k_attack_animation)
        {
            m_is_locked_on_attack = false;
        }
    }
    
    void client_main_character_controller::on_died(const gb::ces_entity_shared_ptr& owner)
    {
        client_base_character_controller::on_died(owner);
        auto character_battle_component = client_main_character_controller::get_component<ces_character_battle_component>();
        character_battle_component->target = nullptr;
        m_actions_processor->interrupt_all_actions();
        if(m_on_died_callback)
        {
            m_on_died_callback(owner);
        }
    }
    
    void client_main_character_controller::on_killed(const gb::ces_entity_shared_ptr& owner, const gb::ces_entity_shared_ptr& target)
    {
        client_base_character_controller::on_killed(owner, target);
        auto character_battle_component = client_main_character_controller::get_component<ces_character_battle_component>();
        character_battle_component->target = nullptr;
        if(m_on_killed_callback)
        {
            m_on_killed_callback(owner, target);
        }
    }
    
    void client_main_character_controller::set_on_death_cooldown_callback(const on_death_cooldown_callback_t& callback)
    {
        m_on_death_cooldown_callback = callback;
    }
    
    void client_main_character_controller::set_on_died_callback(const on_died_callback_t& callback)
    {
        m_on_died_callback = callback;
    }
    
    void client_main_character_controller::set_on_killed_callback(const on_killed_callback_t& callback)
    {
        m_on_killed_callback = callback;
    }
    
    void client_main_character_controller::set_path_map(const path_map_shared_ptr& path_map)
    {
        m_path_map = path_map;
    }
    
    void client_main_character_controller::on_tap_on_map_at_position(const glm::vec2& end_position)
    {
        auto character_statistic_component = client_base_character_controller::get_component<ces_character_statistic_component>();
        if(!character_statistic_component->is_dead)
        {
            m_actions_processor->interrupt_all_actions();
            glm::vec2 start_position = client_base_character_controller::position;
            std::queue<glm::vec2> move_path = game::pathfinder::find_path(start_position, end_position,
                                                                          m_pathfinder, m_path_map);
            while(!move_path.empty())
            {
                ai_move_action_shared_ptr move_action = std::make_shared<ai_move_action>();
                move_action->set_parameters(std::static_pointer_cast<gb::game_object_2d>(shared_from_this()),
                                            move_path.front());
                move_action->set_in_progress_callback(std::bind(&client_main_character_controller::on_move_action_callback,
                                                                this,
                                                                std::placeholders::_1));
                m_actions_processor->add_action(move_action);
                move_path.pop();
            }
            auto character_battle_component = client_main_character_controller::get_component<ces_character_battle_component>();
            character_battle_component->target = nullptr;
        }
    }
    
    void client_main_character_controller::on_tap_on_character(const gb::ces_entity_shared_ptr& target)
    {
        auto character_battle_component = client_main_character_controller::get_component<ces_character_battle_component>();
        character_battle_component->target = target;
    }
    
    void client_main_character_controller::on_tap_on_attack_button(const gb::ces_entity_shared_ptr&)
    {
        auto character_battle_component = client_main_character_controller::get_component<ces_character_battle_component>();
        gb::ces_entity_shared_ptr target = character_battle_component->target;
        if(target)
        {
            auto character_statistic_component = client_main_character_controller::get_component<ces_character_statistic_component>();
            auto target_transformation_component = target->get_component<gb::ces_transformation_2d_component>();
            glm::vec2 target_position = target_transformation_component->get_position();
            glm::vec2 current_position = client_main_character_controller::position;
            f32 distance = glm::distance(current_position, target_position);
            f32 attack_distance = character_statistic_component->attack_distance;
            if(distance <= attack_distance)
            {
                gb::ces_entity_shared_ptr light_source_entity = m_character->get_child(character::parts::k_light_source_part, true);
                gb::mesh_2d_shared_ptr light_source_mesh = light_source_entity->get_component<gb::ces_light_mask_component>()->get_mesh();
                
                gb::ces_entity_shared_ptr bounds_entity = target->get_child(character::parts::k_bounds_part, true);
                gb::mesh_2d_shared_ptr bounds_mesh = bounds_entity->get_component<gb::ces_geometry_component>()->get_mesh();
                
                if(gb::mesh_2d::intersect(bounds_mesh->get_vbo(),
                                          bounds_mesh->get_ibo(),
                                          target_transformation_component->get_matrix_m(),
                                          true,
                                          light_source_mesh->get_vbo(),
                                          light_source_mesh->get_ibo(),
                                          glm::mat4(1.f),
                                          false,
                                          nullptr,
                                          nullptr))
                {
                    m_actions_processor->interrupt_all_actions();
                    ai_attack_action_shared_ptr attack_action = std::make_shared<ai_attack_action>();
                    attack_action->set_parameters(std::static_pointer_cast<gb::game_object_2d>(shared_from_this()),
                                                  std::static_pointer_cast<gb::game_object_2d>(target),
                                                  64.f);
                    attack_action->set_in_progress_callback(std::bind(&client_main_character_controller::on_attack_action_callback,
                                                                      this, std::placeholders::_1));
                    m_actions_processor->add_action(attack_action);
                }
            }
            else
            {
                glm::vec2 current_position = client_main_character_controller::position;
                current_position.y -= 48.f;
                f32 current_rotation = client_main_character_controller::rotation;
                auto information_bubble_controller = m_information_bubble_controller.lock();
                information_bubble_controller->push_bubble("I need to be closer", glm::u8vec4(255, 255, 0, 255),
                                                           current_position,
                                                           current_rotation,
                                                           3);
                
                m_actions_processor->interrupt_all_actions();
                ai_chase_action_shared_ptr chase_action = std::make_shared<ai_chase_action>();
                chase_action->set_parameters(std::static_pointer_cast<gb::game_object_2d>(shared_from_this()),
                                             std::static_pointer_cast<gb::game_object_2d>(target),
                                             64.f,
                                             256.f,
                                             m_path_map,
                                             m_pathfinder);
                chase_action->set_in_progress_callback(std::bind(&client_main_character_controller::on_move_action_callback,
                                                                 this,
                                                                 std::placeholders::_1));
                chase_action->set_end_callback([=](const ai_action_shared_ptr& action) {
                    client_main_character_controller::on_tap_on_attack_button(nullptr);
                });
                
                m_actions_processor->add_action(chase_action);
            }
        }
        else
        {
            glm::vec2 current_position = client_main_character_controller::position;
            current_position.y -= 48.f;
            f32 current_rotation = client_main_character_controller::rotation;
            auto information_bubble_controller = m_information_bubble_controller.lock();
            information_bubble_controller->push_bubble("I need a target", glm::u8vec4(255, 255, 0, 255),
                                                       current_position,
                                                       current_rotation,
                                                       3);
        }
    }
    
    void client_main_character_controller::on_tap_on_ability_1_button(const gb::ces_entity_shared_ptr&)
    {
        
    }
    
    void client_main_character_controller::on_tap_on_ability_2_button(const gb::ces_entity_shared_ptr&)
    {
        
    }
    
    void client_main_character_controller::on_tap_on_ability_3_button(const gb::ces_entity_shared_ptr&)
    {
        
    }
}

