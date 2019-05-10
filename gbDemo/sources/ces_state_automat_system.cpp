//
//  ces_state_automat_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/25/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_state_automat_system.h"
#include "ces_scene_state_automat_component.h"
#include "ces_scene_visual_effects_component.h"
#include "ces_level_descriptor_component.h"
#include "ces_car_input_component.h"
#include "ces_ai_car_input_component.h"
#include "ces_system_modifiers_component.h"
#include "ces_car_parts_component.h"
#include "ces_scene_fabricator_component.h"
#include "ces_car_camera_follow_component.h"
#include "ces_level_route_component.h"
#include "ces_car_simulator_system.h"
#include "ces_interaction_system.h"
#include "ces_ai_system.h"
#include "gameplay_fabricator.h"
#include "gameplay_ui_fabricator.h"
#include "game_object_3d.h"
#include "camera_3d.h"

namespace game
{
    ces_state_automat_system::ces_state_automat_system()
    {
        ces_base_system::add_required_component_guid(m_state_automat_components_mask, ces_scene_state_automat_component::class_guid());
        ces_base_system::add_required_components_mask(m_state_automat_components_mask);
        
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_descriptor_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_main_car_components_mask, ces_car_input_component::class_guid());
        ces_base_system::add_required_components_mask(m_main_car_components_mask);
        
        ces_base_system::add_required_component_guid(m_ai_car_components_mask, ces_ai_car_input_component::class_guid());
        ces_base_system::add_required_components_mask(m_ai_car_components_mask);
    }
    
    ces_state_automat_system::~ces_state_automat_system()
    {
        
    }
    
    void ces_state_automat_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_state_automat_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_state_automat_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            const auto scene_state_automat_component = entity->get_component<ces_scene_state_automat_component>();
            const auto scene_visual_effects_component = entity->get_component<ces_scene_visual_effects_component>();
            const auto system_modifiers_component = entity->get_component<gb::ces_system_modifiers_component>();
            const auto scene_fabricator_component = entity->get_component<ces_scene_fabricator_component>();
            
            if (scene_state_automat_component->state == ces_scene_state_automat_component::e_state_none)
            {
                if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_main_menu)
                {
                    scene_visual_effects_component->is_noises_enabled = true;
                }
                else if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_in_game)
                {
                    if (!m_level.expired())
                    {
                        const auto level_descriptor_component = m_level.lock()->get_component<ces_level_descriptor_component>();
                        if (level_descriptor_component->is_started)
                        {
                            f32 start_timestamp = level_descriptor_component->start_timestamp;
                            f32 delta = std::get_tick_count() - start_timestamp;
                            delta /= 1000.f;
                            f32 countdown_time = level_descriptor_component->countdown_time;
                            f32 current_countdown_time = countdown_time - delta;
                            level_descriptor_component->current_countdown_time = current_countdown_time;
                            
                            if (current_countdown_time <= 0.f)
                            {
                                system_modifiers_component->pause_system(ces_car_simulator_system::class_guid(), false);
                                system_modifiers_component->pause_system(ces_interaction_system::class_guid(), false);
                                system_modifiers_component->pause_system(ces_ai_system::class_guid(), false);
                                
                                scene_visual_effects_component->is_noises_enabled = false;
                                
                                if (!m_main_car.expired())
                                {
                                    const auto car_parts_component = m_main_car.lock()->get_component<ces_car_parts_component>();
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_label)->visible = true;
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_value_label)->visible = true;
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_label)->visible = true;
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_value_label)->visible = true;
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_label)->visible = true;
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_value_label)->visible = true;
                                    car_parts_component->get_part(ces_car_parts_component::parts::k_ui_direction_arrow)->visible = true;
                                }
                            }
                        }
                    }
                }
            }
            else if (scene_state_automat_component->state == ces_scene_state_automat_component::e_state_should_preload)
            {
                scene_state_automat_component->state = ces_scene_state_automat_component::e_state_pre_loading;
                scene_visual_effects_component->is_crossfade_enabled = true;
                scene_visual_effects_component->crossfade_progress = 0.f;
            }
            else if (scene_state_automat_component->state == ces_scene_state_automat_component::e_state_pre_loading)
            {
                f32 loading_progress = scene_state_automat_component->loading_progress;
                scene_visual_effects_component->is_crossfade_enabled = true;
                scene_visual_effects_component->crossfade_progress = loading_progress;
                scene_visual_effects_component->is_noises_enabled = false;
                
                if (loading_progress <= 1.f)
                {
                    loading_progress += ces_scene_state_automat_component::k_loading_progress_inc_value;
                }
                else
                {
                    loading_progress = 1.f;
                    
                    if (!m_level.expired())
                    {
                        m_level.lock()->remove_from_parent();
                        m_level.reset();
                    }
                    
                    if(!m_main_car.expired())
                    {
                        m_main_car.lock()->remove_from_parent();
                        m_main_car.reset();
                    }
                    
                    for (auto ai_car_it : m_ai_cars)
                    {
                        if (!ai_car_it.second.expired())
                        {
                            ai_car_it.second.lock()->remove_from_parent();
                            ai_car_it.second.reset();
                        }
                    }
                    m_ai_cars.clear();
                    
                    const gameplay_fabricator_shared_ptr gameplay_fabricator = scene_fabricator_component->gameplay_fabricator;
                    const gameplay_ui_fabricator_shared_ptr gameplay_ui_fabricator = scene_fabricator_component->gameplay_ui_fabricator;
                    
                    if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_main_menu)
                    {
                        
                    }
                    else if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_in_game)
                    {
                        const auto level = gameplay_fabricator->create_scene("track_output.tmx");
                        root->add_child(level);
                        
                        const auto main_car = gameplay_fabricator->create_player_car("character.human_01.xml");
                        gameplay_fabricator->place_car_on_level(level, main_car, 0);
                        main_car->add_component(std::make_shared<ces_car_camera_follow_component>());
                        root->add_child(main_car);
                        
                        const auto car_parts_component = main_car->get_component<ces_car_parts_component>();
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_label)->visible = false;
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_value_label)->visible = false;
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_label)->visible = false;
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_value_label)->visible = false;
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_label)->visible = false;
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_value_label)->visible = false;
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_direction_arrow)->visible = false;
                        
                        glm::vec3 main_car_rotation = main_car->rotation;
                        get_current_camera_3d()->set_rotation(main_car_rotation.y - 90.f);
                        
                        const auto ai_car_01 = gameplay_fabricator->create_opponent_car("character.human_01.xml");
                        gameplay_fabricator->place_car_on_level(level, ai_car_01, 1);
                        root->add_child(ai_car_01);
                        
                        const auto ai_car_02 = gameplay_fabricator->create_opponent_car("character.human_01.xml");
                        gameplay_fabricator->place_car_on_level(level, ai_car_02, 2);
                        root->add_child(ai_car_02);
                        
                        const auto ai_car_03 = gameplay_fabricator->create_opponent_car("character.human_01.xml");
                        gameplay_fabricator->place_car_on_level(level, ai_car_03, 3);
                        root->add_child(ai_car_03);
                        
                        system_modifiers_component->pause_system(ces_car_simulator_system::class_guid(), true);
                        system_modifiers_component->pause_system(ces_interaction_system::class_guid(), true);
                        system_modifiers_component->pause_system(ces_ai_system::class_guid(), true);
                        
                        const auto level_descriptor_component = level->get_component<ces_level_descriptor_component>();
                        level_descriptor_component->start_timestamp = std::get_tick_count();
                        level_descriptor_component->is_started = true;
                    }
                    scene_state_automat_component->state = ces_scene_state_automat_component::e_state_loading;
                }
                scene_state_automat_component->loading_progress = loading_progress;
            }
            else if (scene_state_automat_component->state == ces_scene_state_automat_component::e_state_loading)
            {
                f32 loading_progress = scene_state_automat_component->loading_progress;
                scene_visual_effects_component->is_crossfade_enabled = true;
                scene_visual_effects_component->crossfade_progress = loading_progress;
                scene_visual_effects_component->is_noises_enabled = false;
                
                if (loading_progress >= 0.f)
                {
                    loading_progress -= ces_scene_state_automat_component::k_loading_progress_inc_value;
                }
                else
                {
                    scene_visual_effects_component->is_crossfade_enabled = false;
                    scene_visual_effects_component->is_noises_enabled = true;
                    loading_progress = 0.f;
                    scene_state_automat_component->state = ces_scene_state_automat_component::e_state_none;
                }
                scene_state_automat_component->loading_progress = loading_progress;
            }
        });
        
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_main_car_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            m_main_car = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_ai_car_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            std::string id = entity->tag;
            m_ai_cars[id] = entity;
        });
    }
    
    void ces_state_automat_system::on_feed_end(f32 dt)
    {
        
    }
}
