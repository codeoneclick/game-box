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
#include "ces_ui_interaction_component.h"
#include "ces_scene_fabricator_component.h"
#include "ces_car_camera_follow_component.h"
#include "ces_box2d_world_component.h"
#include "ces_level_route_component.h"
#include "ces_car_simulator_system.h"
#include "ces_interaction_system.h"
#include "ces_ai_system.h"
#include "gameplay_fabricator.h"
#include "gameplay_ui_fabricator.h"
#include "game_object_3d.h"
#include "camera_3d.h"
#include "button.h"
#include "image_button.h"

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
        
        ces_base_system::add_required_component_guid(m_ui_components_mask, ces_ui_interaction_component::class_guid());
        ces_base_system::add_required_components_mask(m_ui_components_mask);
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
            
            const gameplay_fabricator_shared_ptr gameplay_fabricator = scene_fabricator_component->gameplay_fabricator;
            const gameplay_ui_fabricator_shared_ptr gameplay_ui_fabricator = scene_fabricator_component->gameplay_ui_fabricator;
            
            if (scene_state_automat_component->state == ces_scene_state_automat_component::e_state_none)
            {
                if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_main_menu)
                {
                    scene_visual_effects_component->is_noises_enabled = true;
                    root->get_component<gb::ces_box2d_world_component>()->set_update_interval(1.f / 60.f);
                }
                if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_garage)
                {
                    scene_visual_effects_component->is_noises_enabled = false;
                    root->get_component<gb::ces_box2d_world_component>()->set_update_interval(1.f / 240.f);
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
                    
                    if (!m_open_levels_list_dialog_button.expired())
                    {
                        const auto open_levels_list_dialog_button = std::static_pointer_cast<gb::ui::image_button>(m_open_levels_list_dialog_button.lock());
                        glm::vec2 position = open_levels_list_dialog_button->position;
                        glm::vec2 size = open_levels_list_dialog_button->size;
                        
                        position.x = glm::mix(position.x, -size.x, 1.f - loading_progress);
                        open_levels_list_dialog_button->position = position;
                    }
                    
                    if (!m_open_garage_button.expired())
                    {
                        const auto open_garage_button = std::static_pointer_cast<gb::ui::image_button>(m_open_garage_button.lock());
                        glm::vec2 position = open_garage_button->position;
                        glm::vec2 size = open_garage_button->size;
                        
                        position.x = glm::mix(position.x, -size.x, 1.f - loading_progress);
                        open_garage_button->position = position;
                    }
                    
                    if (!m_back_from_garage_button.expired())
                    {
                        const auto back_from_garage_button = std::static_pointer_cast<gb::ui::image_button>(m_back_from_garage_button.lock());
                        glm::vec2 position = back_from_garage_button->position;
                        glm::vec2 size = back_from_garage_button->size;
                        
                        position.x = glm::mix(position.x, -size.x, 1.f - loading_progress);
                        back_from_garage_button->position = position;
                    }
                    
                    if (!m_next_car_in_garage_button.expired())
                    {
                        const auto next_car_in_garage_button = std::static_pointer_cast<gb::ui::image_button>(m_next_car_in_garage_button.lock());
                        glm::vec2 position = next_car_in_garage_button->position;
                        glm::vec2 size = next_car_in_garage_button->size;
                        
                        position.x = glm::mix(position.x, gameplay_ui_fabricator->get_screen_size().x + size.x, 1.f - loading_progress);
                        next_car_in_garage_button->position = position;
                    }
                    
                    if (!m_prev_car_in_garage_button.expired())
                    {
                        const auto prev_car_in_garage_button = std::static_pointer_cast<gb::ui::image_button>(m_prev_car_in_garage_button.lock());
                        glm::vec2 position = prev_car_in_garage_button->position;
                        glm::vec2 size = prev_car_in_garage_button->size;
                        
                        position.x = glm::mix(position.x, -size.x, 1.f - loading_progress);
                        prev_car_in_garage_button->position = position;
                    }
                }
                else
                {
                    loading_progress = 1.f;
                    
                    if (!m_open_levels_list_dialog_button.expired())
                    {
                        m_open_levels_list_dialog_button.lock()->remove_from_parent();
                        m_open_levels_list_dialog_button.reset();
                    }
                    
                    if (!m_open_garage_button.expired())
                    {
                        m_open_garage_button.lock()->remove_from_parent();
                        m_open_garage_button.reset();
                    }
                    
                    if (!m_back_from_garage_button.expired())
                    {
                        m_back_from_garage_button.lock()->remove_from_parent();
                        m_back_from_garage_button.reset();
                    }
                    
                    if (!m_levels_list_dialog.expired())
                    {
                        m_levels_list_dialog.lock()->remove_from_parent();
                        m_levels_list_dialog.reset();
                    }
                    
                    if (!m_next_car_in_garage_button.expired())
                    {
                        m_next_car_in_garage_button.lock()->remove_from_parent();
                        m_next_car_in_garage_button.reset();
                    }
                    
                    if (!m_prev_car_in_garage_button.expired())
                    {
                        m_prev_car_in_garage_button.lock()->remove_from_parent();
                        m_prev_car_in_garage_button.reset();
                    }
                    
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
                    
                    if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_main_menu)
                    {
                        const auto level = gameplay_fabricator->create_scene("track_output.tmx");
                        root->add_child(level);
                        
                        const auto main_car = gameplay_fabricator->create_opponent_car("car_01");
                        gameplay_fabricator->place_car_on_level(level, main_car, 0);
                        main_car->add_component(std::make_shared<ces_car_camera_follow_component>());
                        main_car->get_component<ces_car_camera_follow_component>()->is_preview_mode = false;
                        root->add_child(main_car);
                        
                        const auto car_parts_component = main_car->get_component<ces_car_parts_component>();
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_name_label)->visible = false;
                        
                        glm::vec3 main_car_rotation = main_car->rotation;
                        get_current_camera_3d()->set_rotation(main_car_rotation.y - 90.f);
                        
                        const auto open_levels_list_dialog_button = gameplay_ui_fabricator->create_open_levels_list_dialog_button("");
                        root->add_child(open_levels_list_dialog_button);
                        
                        glm::vec2 position = open_levels_list_dialog_button->position;
                        glm::vec2 size = open_levels_list_dialog_button->size;
                        position.x = -size.x;
                        open_levels_list_dialog_button->position = position;
                        
                        const auto open_garage_button = gameplay_ui_fabricator->create_open_garage_button("");
                        root->add_child(open_garage_button);
                        
                        position = open_garage_button->position;
                        size = open_garage_button->size;
                        position.x = -size.x;
                        open_garage_button->position = position;
                        
                        const auto levels_list_dialog = gameplay_ui_fabricator->create_levels_list_dialog("");
                        root->add_child(levels_list_dialog);
                    }
                    else if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_garage)
                    {
                        const auto level = gameplay_fabricator->create_scene("garage_scene.tmx");
                        root->add_child(level);
                        
                        const auto main_car = gameplay_fabricator->create_opponent_car("car_01");
                        gameplay_fabricator->place_car_on_level(level, main_car, 0);
                        main_car->add_component(std::make_shared<ces_car_camera_follow_component>());
                        main_car->get_component<ces_car_camera_follow_component>()->is_preview_mode = true;
                        root->add_child(main_car);
                        
                        const auto car_parts_component = main_car->get_component<ces_car_parts_component>();
                        car_parts_component->get_part(ces_car_parts_component::parts::k_ui_name_label)->visible = false;
                        
                        glm::vec3 main_car_rotation = main_car->rotation;
                        get_current_camera_3d()->set_rotation(main_car_rotation.y + 45.f);
                        
                        const auto back_from_garage_button = gameplay_ui_fabricator->create_back_from_garage_button("");
                        root->add_child(back_from_garage_button);
                        
                        glm::vec2 position = back_from_garage_button->position;
                        glm::vec2 size = back_from_garage_button->size;
                        position.x = -size.x;
                        back_from_garage_button->position = position;
                        
                        const auto next_car_in_garage_button = gameplay_ui_fabricator->create_next_car_in_garage_button("");
                        root->add_child(next_car_in_garage_button);
                        
                        position = next_car_in_garage_button->position;
                        size = next_car_in_garage_button->size;
                        position.x = gameplay_ui_fabricator->get_screen_size().x + size.x;
                        next_car_in_garage_button->position = position;
                        
                        const auto prev_car_in_garage_button = gameplay_ui_fabricator->create_prev_car_in_garage_button("");
                        root->add_child(prev_car_in_garage_button);
                        
                        position = prev_car_in_garage_button->position;
                        size = prev_car_in_garage_button->size;
                        position.x = -size.x;
                        prev_car_in_garage_button->position = position;
                    }
                    else if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_in_game)
                    {
                        const auto level = gameplay_fabricator->create_scene("track_output.tmx");
                        root->add_child(level);
                        
                        const auto main_car = gameplay_fabricator->create_player_car("car_01");
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
                        
                        const auto ai_car_01 = gameplay_fabricator->create_opponent_car("car_01");
                        gameplay_fabricator->place_car_on_level(level, ai_car_01, 1);
                        root->add_child(ai_car_01);
                        
                        const auto ai_car_02 = gameplay_fabricator->create_opponent_car("car_01");
                        gameplay_fabricator->place_car_on_level(level, ai_car_02, 2);
                        root->add_child(ai_car_02);
                        
                        const auto ai_car_03 = gameplay_fabricator->create_opponent_car("car_01");
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
                    
                    if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_main_menu)
                    {
                        if (!m_open_levels_list_dialog_button.expired())
                        {
                            const auto open_levels_list_dialog_button = std::static_pointer_cast<gb::ui::image_button>(m_open_levels_list_dialog_button.lock());
                            glm::vec2 position = open_levels_list_dialog_button->position;
                            glm::vec2 size = open_levels_list_dialog_button->size;
                            
                            position.x = glm::mix(-size.x, gameplay_ui_fabricator::k_open_levels_list_dialog_button_position.x, 1.f - loading_progress);
                            open_levels_list_dialog_button->position = position;
                        }
                        
                        if (!m_open_garage_button.expired())
                        {
                            const auto open_garage_button = std::static_pointer_cast<gb::ui::image_button>(m_open_garage_button.lock());
                            glm::vec2 position = open_garage_button->position;
                            glm::vec2 size = open_garage_button->size;
                            
                            position.x = glm::mix(-size.x, gameplay_ui_fabricator::k_open_garage_button_position.x, 1.f - loading_progress);
                            open_garage_button->position = position;
                        }
                    }
                    else if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_garage)
                    {
                        if (!m_back_from_garage_button.expired())
                        {
                            const auto back_from_garage_button = std::static_pointer_cast<gb::ui::image_button>(m_back_from_garage_button.lock());
                            glm::vec2 position = back_from_garage_button->position;
                            glm::vec2 size = back_from_garage_button->size;
                            
                            position.x = glm::mix(-size.x, gameplay_ui_fabricator::k_back_from_garage_button_position.x, 1.f - loading_progress);
                            back_from_garage_button->position = position;
                        }
                        
                        if (!m_next_car_in_garage_button.expired())
                        {
                            const auto next_car_in_garage_button = std::static_pointer_cast<gb::ui::image_button>(m_next_car_in_garage_button.lock());
                            glm::vec2 position = next_car_in_garage_button->position;
                            glm::vec2 size = next_car_in_garage_button->size;
                            
                            position.x = glm::mix(gameplay_ui_fabricator->get_screen_size().x + size.x, gameplay_ui_fabricator::k_next_car_in_garage_button_position.x, 1.f - loading_progress);
                            next_car_in_garage_button->position = position;
                        }
                        
                        if (!m_prev_car_in_garage_button.expired())
                        {
                            const auto prev_car_in_garage_button = std::static_pointer_cast<gb::ui::image_button>(m_prev_car_in_garage_button.lock());
                            glm::vec2 position = prev_car_in_garage_button->position;
                            glm::vec2 size = prev_car_in_garage_button->size;
                            
                            position.x = glm::mix(-size.x, gameplay_ui_fabricator::k_prev_car_in_garage_button_position.x, 1.f - loading_progress);
                            prev_car_in_garage_button->position = position;
                        }
                    }
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
        
        ces_base_system::enumerate_entities_with_components(m_ui_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            auto ui_interaction_component = entity->get_component<ces_ui_interaction_component>();
            switch (ui_interaction_component->get_ui())
            {
                case ces_ui_interaction_component::e_ui_open_levels_list_dialog_button:
                {
                    m_open_levels_list_dialog_button = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_open_garage_button:
                {
                    m_open_garage_button = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_back_from_garage_button:
                {
                    m_back_from_garage_button = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_levels_list_dialog:
                {
                    m_levels_list_dialog = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_next_car_in_garage_button:
                {
                    m_next_car_in_garage_button = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_prev_car_in_garage_button:
                {
                    m_prev_car_in_garage_button = entity;
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
