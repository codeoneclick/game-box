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
#include "ces_car_ai_input_component.h"
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
#include "ces_box2d_system.h"
#include "ces_car_sound_system.h"
#include "gameplay_fabricator.h"
#include "gameplay_ui_fabricator.h"
#include "game_object_3d.h"
#include "camera_3d.h"
#include "button.h"
#include "textfield.h"
#include "image_button.h"
#include "ces_garage_database_component.h"
#include "ces_car_drift_state_component.h"
#include "ces_level_tutorial_component.h"
#include "ces_sound_component.h"
#include "ces_car_sounds_set_component.h"

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
        
        ces_base_system::add_required_component_guid(m_ai_car_components_mask, ces_car_ai_input_component::class_guid());
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
                if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_in_game)
                {
                   
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
                        if (level_descriptor_component->is_started && !level_descriptor_component->is_paused && !level_descriptor_component->is_finished)
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
                                system_modifiers_component->pause_system(gb::ces_box2d_system::class_guid(), false);
                                system_modifiers_component->pause_system(ces_car_sound_system::class_guid(), false);
                                
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
                        else if (level_descriptor_component->is_paused)
                        {
                            system_modifiers_component->pause_system(ces_car_simulator_system::class_guid(), true);
                            system_modifiers_component->pause_system(ces_interaction_system::class_guid(), true);
                            system_modifiers_component->pause_system(ces_ai_system::class_guid(), true);
                            system_modifiers_component->pause_system(gb::ces_box2d_system::class_guid(), true);
                            system_modifiers_component->pause_system(ces_car_sound_system::class_guid(), true);
                            
                            scene_visual_effects_component->is_noises_enabled = true;
                            
                            const auto sound_component = m_main_car.lock()->get_component<gb::al::ces_sound_component>();
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_idle, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_low, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_mid, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_high, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_low, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_mid, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_high, 0.f);
                        }
                        else if (level_descriptor_component->is_finished)
                        {
                            scene_visual_effects_component->is_noises_enabled = true;
                            
                            const auto sound_component = m_main_car.lock()->get_component<gb::al::ces_sound_component>();
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_idle, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_low, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_mid, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_off_high, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_low, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_mid, 0.f);
                            sound_component->set_volume(ces_car_sounds_set_component::sounds::k_engine_on_high, 0.f);
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
                    
                    if (!m_goto_racing_button.expired())
                    {
                        const auto goto_racing_button = std::static_pointer_cast<gb::ui::image_button>(m_goto_racing_button.lock());
                        glm::vec2 position = goto_racing_button->position;
                        glm::vec2 size = goto_racing_button->size;
                        
                        position.x = glm::mix(position.x, gameplay_ui_fabricator->get_screen_size().x + size.x, 1.f - loading_progress);
                        goto_racing_button->position = position;
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
                    
                    if (!m_car_skin_1_button.expired())
                    {
                        const auto car_skin_1_button = std::static_pointer_cast<gb::ui::image_button>(m_car_skin_1_button.lock());
                        glm::vec2 position = car_skin_1_button->position;
                        glm::vec2 size = car_skin_1_button->size;
                        
                        position.x = glm::mix(position.x, gameplay_ui_fabricator->get_screen_size().x + size.x, 1.f - loading_progress);
                        car_skin_1_button->position = position;
                    }
                    
                    if (!m_car_skin_2_button.expired())
                    {
                        const auto car_skin_2_button = std::static_pointer_cast<gb::ui::image_button>(m_car_skin_2_button.lock());
                        glm::vec2 position = car_skin_2_button->position;
                        glm::vec2 size = car_skin_2_button->size;
                        
                        position.x = glm::mix(position.x, gameplay_ui_fabricator->get_screen_size().x + size.x, 1.f - loading_progress);
                        car_skin_2_button->position = position;
                    }
                    
                    if (!m_car_skin_3_button.expired())
                    {
                        const auto car_skin_3_button = std::static_pointer_cast<gb::ui::image_button>(m_car_skin_3_button.lock());
                        glm::vec2 position = car_skin_3_button->position;
                        glm::vec2 size = car_skin_3_button->size;
                        
                        position.x = glm::mix(position.x, gameplay_ui_fabricator->get_screen_size().x + size.x, 1.f - loading_progress);
                        car_skin_3_button->position = position;
                    }
                    
                    if (!m_tickets_label.expired())
                    {
                        const auto tickets_label = std::static_pointer_cast<gb::ui::textfield>(m_tickets_label.lock());
                        glm::vec2 position = tickets_label->position;
                        glm::vec2 size = tickets_label->size;
                        
                        position.x = glm::mix(position.x, -size.x, 1.f - loading_progress);
                        tickets_label->position = position;
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
                    
                    if (!m_car_skin_1_button.expired())
                    {
                        m_car_skin_1_button.lock()->remove_from_parent();
                        m_car_skin_1_button.reset();
                    }
                    
                    if (!m_car_skin_2_button.expired())
                    {
                        m_car_skin_2_button.lock()->remove_from_parent();
                        m_car_skin_2_button.reset();
                    }
                    
                    if (!m_car_skin_3_button.expired())
                    {
                        m_car_skin_3_button.lock()->remove_from_parent();
                        m_car_skin_3_button.reset();
                    }
                    
                    if (!m_goto_racing_button.expired())
                    {
                        m_goto_racing_button.lock()->remove_from_parent();
                        m_goto_racing_button.reset();
                    }
                    
                    if (!m_pause_button.expired())
                    {
                        m_pause_button.lock()->remove_from_parent();
                        m_pause_button.reset();
                    }
                    
                    if (!m_pause_menu_dialog.expired())
                    {
                        m_pause_menu_dialog.lock()->remove_from_parent();
                        m_pause_menu_dialog.reset();
                    }
                    
                    if (!m_quit_dialog.expired())
                    {
                        m_quit_dialog.lock()->remove_from_parent();
                        m_quit_dialog.reset();
                    }
                    
                    if (!m_restart_dialog.expired())
                    {
                        m_restart_dialog.lock()->remove_from_parent();
                        m_restart_dialog.reset();
                    }
                    
                    if (!m_cars_list_dialog.expired())
                    {
                        m_cars_list_dialog.lock()->remove_from_parent();
                        m_cars_list_dialog.reset();
                    }
                    
                    if (!m_tickets_label.expired())
                    {
                        m_tickets_label.lock()->remove_from_parent();
                        m_tickets_label.reset();
                    }
                    
                    if (!m_countdown_timer_label.expired())
                    {
                        m_countdown_timer_label.lock()->remove_from_parent();
                        m_countdown_timer_label.reset();
                    }
                    
                    
                    if (!m_end_game_dialog.expired())
                    {
                        m_end_game_dialog.lock()->remove_from_parent();
                        m_end_game_dialog.reset();
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
                    
                    for (auto ai_car_it : m_all_cars)
                    {
                        if (!ai_car_it.second.expired())
                        {
                            ai_car_it.second.lock()->remove_from_parent();
                            ai_car_it.second.reset();
                        }
                    }
                    m_all_cars.clear();
                    
                    if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_main_menu)
                    {
                        system_modifiers_component->pause_system(ces_car_simulator_system::class_guid(), false);
                        system_modifiers_component->pause_system(ces_interaction_system::class_guid(), false);
                        system_modifiers_component->pause_system(ces_ai_system::class_guid(), false);
                        system_modifiers_component->pause_system(gb::ces_box2d_system::class_guid(), false);
                        system_modifiers_component->pause_system(ces_car_sound_system::class_guid(), false);
                        
                        const auto level = gameplay_fabricator->create_scene("track_output.tmx");
                        root->add_child(level);
                        
                        const auto garage_database_component = level->get_component<ces_garage_database_component>();
                        const auto selected_car = garage_database_component->get_selected_car(1);
                        
                        std::stringstream selected_car_configuration_filename;
                        selected_car_configuration_filename<<"car_0";
                        selected_car_configuration_filename<<selected_car->get_id();
                        const auto main_car = gameplay_fabricator->create_ai_car(selected_car_configuration_filename.str());
                        gameplay_fabricator->place_car_on_level(level, main_car, 0);
                        main_car->add_component(std::make_shared<ces_car_camera_follow_component>());
                        main_car->get_component<ces_car_camera_follow_component>()->is_preview_mode = false;
                        gameplay_fabricator->reskin_car(main_car, selected_car_configuration_filename.str(), selected_car->get_skin_index());
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
                        
                        const auto goto_racing_button = gameplay_ui_fabricator->create_goto_racing_button("");
                        root->add_child(goto_racing_button);
                        
                        position = goto_racing_button->position;
                        size = goto_racing_button->size;
                        position.x = gameplay_ui_fabricator->get_screen_size().x + size.x;
                        goto_racing_button->position = position;
                        
                        const auto levels_list_dialog = gameplay_ui_fabricator->create_levels_list_dialog("");
                        root->add_child(levels_list_dialog);
                        
                        const auto tickets_label = gameplay_ui_fabricator->create_tickets_label("");
                        root->add_child(tickets_label);
                        
                        position = tickets_label->position;
                        size = tickets_label->size;
                        position.x = -size.x;
                        tickets_label->position = position;
                    }
                    else if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_garage)
                    {
                        const auto level = gameplay_fabricator->create_scene("garage_scene.tmx");
                        root->add_child(level);
                        
                        const auto garage_database_component = level->get_component<ces_garage_database_component>();
                        const auto selected_car = garage_database_component->get_selected_car(1);
                        
                        std::stringstream selected_car_configuration_filename;
                        selected_car_configuration_filename<<"car_0";
                        selected_car_configuration_filename<<selected_car->get_id();
                        const auto main_car = gameplay_fabricator->create_ai_car(selected_car_configuration_filename.str());
                        gameplay_fabricator->place_car_on_level(level, main_car, 0);
                        main_car->add_component(std::make_shared<ces_car_camera_follow_component>());
                        main_car->get_component<ces_car_camera_follow_component>()->is_preview_mode = true;
                        gameplay_fabricator->reskin_car(main_car, selected_car_configuration_filename.str(), selected_car->get_skin_index());
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
                        
                        const auto car_skin_1_button = gameplay_ui_fabricator->create_car_skin_1_button("");
                        root->add_child(car_skin_1_button);
                        
                        position = car_skin_1_button->position;
                        size = car_skin_1_button->size;
                        position.x = gameplay_ui_fabricator->get_screen_size().x + size.x;
                        car_skin_1_button->position = position;
                        
                        const auto car_skin_2_button = gameplay_ui_fabricator->create_car_skin_2_button("");
                        root->add_child(car_skin_2_button);
                        
                        position = car_skin_2_button->position;
                        size = car_skin_2_button->size;
                        position.x = gameplay_ui_fabricator->get_screen_size().x + size.x;
                        car_skin_2_button->position = position;
                        
                        const auto car_skin_3_button = gameplay_ui_fabricator->create_car_skin_3_button("");
                        root->add_child(car_skin_3_button);
                        
                        position = car_skin_3_button->position;
                        size = car_skin_3_button->size;
                        position.x = gameplay_ui_fabricator->get_screen_size().x + size.x;
                        car_skin_3_button->position = position;
                        
                        const auto tickets_label = gameplay_ui_fabricator->create_tickets_label("");
                        root->add_child(tickets_label);
                        
                        position = tickets_label->position;
                        size = tickets_label->size;
                        position.x = -size.x;
                        tickets_label->position = position;
                    }
                    else if (scene_state_automat_component->mode == ces_scene_state_automat_component::e_mode_in_game)
                    {
                        system_modifiers_component->pause_system(ces_car_simulator_system::class_guid(), false);
                        system_modifiers_component->pause_system(ces_interaction_system::class_guid(), false);
                        system_modifiers_component->pause_system(ces_ai_system::class_guid(), false);
                        system_modifiers_component->pause_system(gb::ces_box2d_system::class_guid(), false);
                        system_modifiers_component->pause_system(ces_car_sound_system::class_guid(), false);
                        
                        const auto level = gameplay_fabricator->create_scene("track_output.tmx");
                        const auto level_tutorial_component = std::make_shared<ces_level_tutorial_component>();
                        level_tutorial_component->set_parameters(ces_level_tutorial_component::e_tutorial_id::e_tutorial_id_steer);
                        level->add_component(level_tutorial_component);
                        root->add_child(level);
                        
                        const auto garage_database_component = level->get_component<ces_garage_database_component>();
                        const auto selected_car = garage_database_component->get_selected_car(1);
                        
                        std::stringstream selected_car_configuration_filename;
                        selected_car_configuration_filename<<"car_0";
                        selected_car_configuration_filename<<selected_car->get_id();
                        
                        const auto main_car = gameplay_fabricator->create_player_car(selected_car_configuration_filename.str());
                        gameplay_fabricator->place_car_on_level(level, main_car, 0);
                        main_car->add_component(std::make_shared<ces_car_camera_follow_component>());
                        gameplay_fabricator->reskin_car(main_car, selected_car_configuration_filename.str(), selected_car->get_skin_index());
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
                        
                        std::stringstream ai_car_01_configuration_filename;
                        ai_car_01_configuration_filename<<"car_0";
                        ai_car_01_configuration_filename<<std::get_random_i(1, 4);
                        
                        const auto ai_car_01 = gameplay_fabricator->create_ai_car(ai_car_01_configuration_filename.str());
                        gameplay_fabricator->place_car_on_level(level, ai_car_01, 1);
                        gameplay_fabricator->reskin_car(ai_car_01, ai_car_01_configuration_filename.str(), std::get_random_i(1, 3));
                        root->add_child(ai_car_01);
                        
                        std::stringstream ai_car_02_configuration_filename;
                        ai_car_02_configuration_filename<<"car_0";
                        ai_car_02_configuration_filename<<std::get_random_i(1, 4);
                        
                        const auto ai_car_02 = gameplay_fabricator->create_ai_car(ai_car_02_configuration_filename.str());
                        gameplay_fabricator->place_car_on_level(level, ai_car_02, 2);
                        gameplay_fabricator->reskin_car(ai_car_02, ai_car_02_configuration_filename.str(), std::get_random_i(1, 3));
                        root->add_child(ai_car_02);
                        
                        std::stringstream ai_car_03_configuration_filename;
                        ai_car_03_configuration_filename<<"car_0";
                        ai_car_03_configuration_filename<<std::get_random_i(1, 4);
                        
                        const auto ai_car_03 = gameplay_fabricator->create_ai_car(ai_car_03_configuration_filename.str());
                        gameplay_fabricator->place_car_on_level(level, ai_car_03, 3);
                        gameplay_fabricator->reskin_car(ai_car_03, ai_car_03_configuration_filename.str(), std::get_random_i(1, 3));
                        root->add_child(ai_car_03);
                        
                        system_modifiers_component->pause_system(ces_car_simulator_system::class_guid(), true);
                        system_modifiers_component->pause_system(ces_interaction_system::class_guid(), true);
                        system_modifiers_component->pause_system(ces_ai_system::class_guid(), true);
                        
                        const auto level_descriptor_component = level->get_component<ces_level_descriptor_component>();
                        level_descriptor_component->start_timestamp = std::get_tick_count();
                        level_descriptor_component->is_started = true;
                        
                        const auto cars_list_dialog = gameplay_ui_fabricator->create_cars_list_dialog("");
                        root->add_child(cars_list_dialog);
                        
                        const auto pause_button = gameplay_ui_fabricator->create_in_game_pause_button("");
                        root->add_child(pause_button);
                        
                        const auto pause_menu_dialog = gameplay_ui_fabricator->create_in_game_pause_menu_dialog("");
                        root->add_child(pause_menu_dialog);
                        
                        const auto restart_dialog = gameplay_ui_fabricator->create_in_game_restart_dialog("");
                        root->add_child(restart_dialog);
                        
                        const auto quit_dialog = gameplay_ui_fabricator->create_in_game_quit_dialog("");
                        root->add_child(quit_dialog);
                        
                        const auto countdown_timer_label = gameplay_ui_fabricator->create_countdown_label("");
                        root->add_child(countdown_timer_label);
                        
                        const auto end_game_dialog = gameplay_ui_fabricator->create_end_game_dialog("");
                        root->add_child(end_game_dialog);
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
                        
                        if (!m_goto_racing_button.expired())
                        {
                            const auto goto_racing_button = std::static_pointer_cast<gb::ui::image_button>(m_goto_racing_button.lock());
                            glm::vec2 position = goto_racing_button->position;
                            glm::vec2 size = goto_racing_button->size;
                            
                            position.x = glm::mix(gameplay_ui_fabricator->get_screen_size().x + size.x, gameplay_ui_fabricator::k_goto_racing_button_position.x, 1.f - loading_progress);
                            goto_racing_button->position = position;
                        }
                        
                        if (!m_tickets_label.expired())
                        {
                            const auto tickets_label = std::static_pointer_cast<gb::ui::textfield>(m_tickets_label.lock());
                            glm::vec2 position = tickets_label->position;
                            glm::vec2 size = tickets_label->size;
                            
                            position.x = glm::mix(-size.x, gameplay_ui_fabricator::k_tickets_label_position.x, 1.f - loading_progress);
                            tickets_label->position = position;
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
                        
                        if (!m_car_skin_1_button.expired())
                        {
                            const auto car_skin_1_button = std::static_pointer_cast<gb::ui::image_button>(m_car_skin_1_button.lock());
                            glm::vec2 position = car_skin_1_button->position;
                            glm::vec2 size = car_skin_1_button->size;
                            
                            position.x = glm::mix(gameplay_ui_fabricator->get_screen_size().x + size.x, gameplay_ui_fabricator::k_car_skin_1_button_position.x, 1.f - loading_progress);
                            car_skin_1_button->position = position;
                        }
                        
                        if (!m_car_skin_2_button.expired())
                        {
                            const auto car_skin_2_button = std::static_pointer_cast<gb::ui::image_button>(m_car_skin_2_button.lock());
                            glm::vec2 position = car_skin_2_button->position;
                            glm::vec2 size = car_skin_2_button->size;
                            
                            position.x = glm::mix(gameplay_ui_fabricator->get_screen_size().x + size.x, gameplay_ui_fabricator::k_car_skin_2_button_position.x, 1.f - loading_progress);
                            car_skin_2_button->position = position;
                        }
                        
                        if (!m_car_skin_3_button.expired())
                        {
                            const auto car_skin_3_button = std::static_pointer_cast<gb::ui::image_button>(m_car_skin_3_button.lock());
                            glm::vec2 position = car_skin_3_button->position;
                            glm::vec2 size = car_skin_3_button->size;
                            
                            position.x = glm::mix(gameplay_ui_fabricator->get_screen_size().x + size.x, gameplay_ui_fabricator::k_car_skin_3_button_position.x, 1.f - loading_progress);
                            car_skin_3_button->position = position;
                        }
                        
                        if (!m_tickets_label.expired())
                        {
                            const auto tickets_label = std::static_pointer_cast<gb::ui::textfield>(m_tickets_label.lock());
                            glm::vec2 position = tickets_label->position;
                            glm::vec2 size = tickets_label->size;
                            
                            position.x = glm::mix(-size.x, gameplay_ui_fabricator::k_tickets_label_position.x, 1.f - loading_progress);
                            tickets_label->position = position;
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
            std::string id = entity->tag;
            m_all_cars[id] = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_ai_car_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            std::string id = entity->tag;
            m_all_cars[id] = entity;
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
                
                case ces_ui_interaction_component::e_ui_car_skin_1_button:
                {
                    m_car_skin_1_button = entity;
                }
                break;
                
                case ces_ui_interaction_component::e_ui_car_skin_2_button:
                {
                    m_car_skin_2_button = entity;
                }
                break;
                
                case ces_ui_interaction_component::e_ui_car_skin_3_button:
                {
                    m_car_skin_3_button = entity;
                }
                break;
                    
                case ces_ui_interaction_component::e_ui_goto_racing_button:
                {
                    m_goto_racing_button = entity;
                }
                break;
                    
                case ces_ui_interaction_component::e_ui_pause_button:
                {
                    m_pause_button = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_pause_menu_dialog:
                {
                    m_pause_menu_dialog = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_restart_dialog:
                {
                    m_restart_dialog = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_quit_dialog:
                {
                    m_quit_dialog = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_cars_list_dialog:
                {
                    m_cars_list_dialog = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_tickets_label:
                {
                    m_tickets_label = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_countdown_label:
                {
                    m_countdown_timer_label = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_end_game_dialog:
                {
                    m_end_game_dialog = entity;
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
