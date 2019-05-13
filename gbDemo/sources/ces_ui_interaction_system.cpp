//
//  ces_ui_interaction_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/27/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ui_interaction_system.h"
#include "ces_character_controllers_component.h"
#include "ces_scene_state_automat_component.h"
#include "ces_character_statistic_component.h"
#include "ces_character_selector_component.h"
#include "ces_car_parts_component.h"
#include "ces_ui_interaction_component.h"
#include "ces_ui_avatar_icon_component.h"
#include "ces_bound_touch_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_light_mask_component.h"
#include "ces_geometry_component.h"
#include "ces_level_controllers_component.h"
#include "ces_level_path_grid_component.h"
#include "shape_3d.h"
#include "label_3d.h"
#include "dialog.h"
#include "button.h"
#include "textfield.h"
#include "joystick.h"
#include "table_view.h"
#include "table_view_cell.h"
#include "action_console.h"
#include "game_object_2d.h"
#include "camera_3d.h"
#include "ai_actions_processor.h"
#include "ai_attack_action.h"
#include "ai_chase_action.h"
#include "information_bubble_controller.h"
#include "ces_character_pathfinder_component.h"
#include "ces_quest_giver_component.h"
#include "ces_quest_receiver_component.h"
#include "ces_ui_quest_dialog_component.h"
#include "ces_ui_questlog_dialog_component.h"
#include "ces_character_navigation_component.h"
#include "ces_box2d_body_component.h"
#include "ces_car_input_component.h"
#include "ces_car_model_component.h"
#include "ces_car_descriptor_component.h"
#include "ces_car_drift_state_component.h"
#include "ces_sound_component.h"
#include "ces_car_gear_component.h"
#include "ces_level_route_component.h"
#include "ces_car_camera_follow_component.h"
#include "ces_level_descriptor_component.h"
#include "levels_list_table_view_cell.h"
#include "ces_scene_state_automat_component.h"
#include "ces_scene_fabricator_component.h"
#include "gameplay_fabricator.h"
#include "gameplay_ui_fabricator.h"

namespace game
{
    ces_ui_interaction_system::ces_ui_interaction_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_route_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_ui_components_mask, ces_ui_interaction_component::class_guid());
        ces_base_system::add_required_components_mask(m_ui_components_mask);
        
        ces_base_system::add_required_component_guid(m_camera_follow_car_components_mask, ces_car_camera_follow_component::class_guid());
        ces_base_system::add_required_components_mask(m_camera_follow_car_components_mask);
        
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_controllers_component::class_guid());
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_statistic_component::class_guid());
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_car_input_component::class_guid());
        ces_base_system::add_required_components_mask(m_character_components_mask);
    }
    
    ces_ui_interaction_system::~ces_ui_interaction_system()
    {
        
    }
    
    void ces_ui_interaction_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_ui_interaction_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        m_scene = root;
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
            
            if (!m_level.expired())
            {
                const auto level_descriptor_component = m_level.lock()->get_component<ces_level_descriptor_component>();
                if (level_descriptor_component)
                {
                    if (!m_main_character.expired())
                    {
                        const auto car = std::static_pointer_cast<gb::game_object_3d>(m_main_character.lock());
                        const auto car_parts_component = car->get_component<ces_car_parts_component>();
                        
                         const auto countdown_value_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_countdown_value_label));
                        
                        f32 current_countdown_time = level_descriptor_component->current_countdown_time;
                        
                        if (current_countdown_time >= 0.f)
                        {
                            i32 seconds = current_countdown_time;
                            f32 f_milliseconds = current_countdown_time - seconds;
                            
                            std::stringstream coundown_value_string_stream;
                            coundown_value_string_stream<<(seconds + 1);
                            countdown_value_label->text = coundown_value_string_stream.str();
                            countdown_value_label->scale = glm::vec3(glm::mix(.1f, .5f, f_milliseconds / 10.f));
                            countdown_value_label->position = glm::vec3(countdown_value_label->get_content_size().x * .5f, 3.f, 4.f);
                        }
                        else
                        {
                            countdown_value_label->text = "GO";
                            countdown_value_label->scale = glm::vec3(.2f);
                            countdown_value_label->position = glm::vec3(countdown_value_label->get_content_size().x * .5f, 3.f, 4.f);
                            glm::u8vec4 font_color = countdown_value_label->font_color;
                            f32 font_color_alpha = font_color.a;
                            font_color.a = glm::mix(font_color_alpha, 0.f, .1f);
                            if (font_color.a <= 0.f)
                            {
                                countdown_value_label->visible = false;
                            }
                            else
                            {
                                countdown_value_label->font_color = font_color;
                                
                                const auto speed_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_label));
                                const auto speed_value_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_value_label));
                                const auto rpm_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_label));
                                const auto rpm_value_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_value_label));
                                
                                font_color.a = 255 - font_color_alpha;
                                speed_label->font_color = font_color;
                                rpm_label->font_color = font_color;
                                
                                font_color.a = std::max(64 - static_cast<i32>(font_color_alpha), 0);
                                speed_value_label->font_color = font_color;
                                rpm_value_label->font_color = font_color;
                            }
                            
                            f32 round_time = level_descriptor_component->round_time;
                            
                            f32 start_timestamp = level_descriptor_component->start_timestamp;
                            f32 delta = std::get_tick_count() - start_timestamp;
                            delta /= 1000.f;
                            f32 current_round_time = round_time - delta;
                            level_descriptor_component->current_round_time = current_round_time;
                            
                            if (!m_countdown_label.expired())
                            {
                                const auto countdown_label = std::static_pointer_cast<gb::ui::textfield>(m_countdown_label.lock());
                                countdown_label->visible = true;
                                i32 seconds = current_round_time;
                                f32 f_milliseconds = current_round_time - seconds;
                                i32 milliseconds = f_milliseconds * 10;
                                    
                                std::stringstream round_time_value_string_stream;
                                round_time_value_string_stream<<(seconds < 10 ? "0" : "")<<seconds<<":"<<milliseconds<<"0 sec";
                                    
                                std::string round_time_text = "TIME: ";
                                round_time_text.append(round_time_value_string_stream.str());
                                countdown_label->set_text(round_time_text);
                            }
                        }
                    }
                }
            }
        });
        
        ces_base_system::enumerate_entities_with_components(m_camera_follow_car_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            m_camera_follow_car = entity;
            
            if (!m_camera_follow_car.expired())
            {
                const auto car = std::static_pointer_cast<gb::game_object_3d>(m_camera_follow_car.lock());
                const auto car_descriptor_component = car->get_component<ces_car_descriptor_component>();
                const auto car_model_component = car->get_component<ces_car_model_component>();
                const auto car_camera_follow_component = car->get_component<ces_car_camera_follow_component>();
                
                glm::vec3 current_position = car->position;
                glm::vec3 current_rotation = car->rotation;
                
                glm::vec2 velocity_wc = car_descriptor_component->velocity_wc;
                f32 velocity_wc_length = glm::length(velocity_wc);
                f32 current_velocity_length_squared = velocity_wc_length * velocity_wc_length;
                f32 max_speed_squared = car_model_component->get_max_speed() * car_model_component->get_max_speed();
                f32 current_speed_factor = glm::clamp(current_velocity_length_squared / max_speed_squared, 0.f, 1.f);
                
                if (car_camera_follow_component->is_preview_mode == false)
                {
                    current_position.x += velocity_wc.x * current_speed_factor * .33f;
                    current_position.z += velocity_wc.y * current_speed_factor * .33f;
                }
                
                const auto camera_3d = ces_base_system::get_current_camera_3d();
                auto current_look_at = camera_3d->get_look_at();
                current_look_at = glm::mix(current_look_at, current_position, glm::clamp(.1f, 1.f, 1.f - current_speed_factor));
                camera_3d->set_look_at(current_look_at);
                
                auto current_camera_rotation = camera_3d->get_rotation();
                if (car_camera_follow_component->is_preview_mode == false)
                {
                    current_camera_rotation = glm::mix(current_camera_rotation, glm::degrees(current_rotation.y) - 90.f, .05f);
                }
                else
                {
                    current_camera_rotation = glm::mix(current_camera_rotation, glm::degrees(current_rotation.y) + 45.f, .05f);
                }
                camera_3d->set_rotation(current_camera_rotation);
                
                f32 min_distance_xz_to_look_at = car_camera_follow_component->min_distance_xz_to_look_at;
                f32 max_distance_xz_to_look_at = car_camera_follow_component->max_distance_xz_to_look_at;
                f32 min_distance_y_to_look_at = car_camera_follow_component->min_distance_y_to_look_at;
                f32 max_distance_y_to_look_at = car_camera_follow_component->max_distance_y_to_look_at;
                camera_3d->set_distance_to_look_at(glm::vec3(glm::mix(min_distance_xz_to_look_at,
                                                                      max_distance_xz_to_look_at,
                                                                      current_speed_factor),
                                                             glm::mix(min_distance_y_to_look_at,
                                                                      max_distance_y_to_look_at,
                                                                      current_speed_factor),
                                                             glm::mix(min_distance_xz_to_look_at,
                                                                      max_distance_xz_to_look_at,
                                                                      current_speed_factor)));
            }
        });
        
        ces_base_system::enumerate_entities_with_components(m_character_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            std::string character_key = entity->tag;
            auto character_statistic_component = entity->get_component<ces_character_statistic_component>();
            if(character_statistic_component->mode == ces_character_statistic_component::e_mode::e_mode_player)
            {
                m_main_character = entity;
                
                const auto main_character = std::static_pointer_cast<gb::game_object_3d>(m_main_character.lock());
                const auto character_navigation_component = main_character->get_component<ces_character_navigation_component>();
                const auto car_descriptor_component = main_character->get_component<ces_car_descriptor_component>();
                const auto car_model_component = main_character->get_component<ces_car_model_component>();
                const auto car_parts_component = main_character->get_component<ces_car_parts_component>();
                const auto car_drift_state_component = main_character->get_component<ces_car_drift_state_component>();
                const auto car_gear_component = main_character->get_component<ces_car_gear_component>();
               
                const auto speed_value_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_value_label));
                const auto rpm_value_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_value_label));
                const auto drift_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_label));
                const auto drift_value_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_value_label));
                
                const auto direction_arrow = std::static_pointer_cast<gb::shape_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_direction_arrow));
                
                glm::vec3 current_position = main_character->position;
                glm::vec3 current_rotation = main_character->rotation;
                
                glm::vec2 velocity_wc = car_descriptor_component->velocity_wc;
                f32 velocity_wc_length = glm::length(velocity_wc);
                f32 current_velocity_length_squared = velocity_wc_length * velocity_wc_length;
                f32 max_speed_squared = car_model_component->get_max_speed() * car_model_component->get_max_speed();
                f32 current_speed_factor = glm::clamp(current_velocity_length_squared / max_speed_squared, 0.f, 1.f);
                
                current_position.x += velocity_wc.x * current_speed_factor * .33f;
                current_position.z += velocity_wc.y * current_speed_factor * .33f;
                
                i32 rpm = car_gear_component->get_rpm(current_speed_factor, car_gear_component->get_previous_load());
                
                std::stringstream string_stream;
                string_stream<<static_cast<i32>(current_speed_factor * 160)<<" km/h";
                speed_value_label->text = string_stream.str();
                
                std::stringstream rpm_string_stream;
                rpm_string_stream<<rpm;
                rpm_value_label->text = rpm_string_stream.str();
                
                if (car_drift_state_component->is_drifting)
                {
                    f32 last_drifring_time = car_drift_state_component->last_drifting_time;
                    f32 current_time = std::get_tick_count();
                    f32 delta = current_time - last_drifring_time;
                    
                    i32 seconds = delta / 1000;
                    f32 f_milliseconds = delta / 1000 - seconds;
                    i32 milliseconds = f_milliseconds * 10;
                    
                    std::stringstream drift_value_string_stream;
                    drift_value_string_stream<<(seconds < 10 ? "0" : "")<<seconds<<":"<<milliseconds<<"0 sec";
                    drift_value_label->text = drift_value_string_stream.str();
                    drift_value_label->font_color = glm::u8vec4(255, 255, 255, glm::min(static_cast<i32>((delta / 1000) * 255), 64));
                    drift_label->font_color = glm::u8vec4(255, 255, 255, glm::min(static_cast<i32>((delta / 1000) * 255), 64));
                }
                else
                {
                    glm::u8vec4 font_color = drift_label->font_color;
                    if (car_drift_state_component->is_interrupted)
                    {
                        font_color.x = 255;
                        font_color.y = 0;
                        font_color.z = 0;
                    }
                    else
                    {
                        font_color.x = 0;
                        font_color.y = 255;
                        font_color.z = 0;
                    }
                    f32 font_color_alpha = font_color.a;
                    font_color.a = glm::mix(font_color_alpha, 0.f, .1f);
                    drift_value_label->font_color = font_color;
                    drift_label->font_color = font_color;
                }
                
                if (!m_level.expired())
                {
                    const auto track_route_component = m_level.lock()->get_component<ces_level_route_component>();
                    std::vector<glm::vec2> route = track_route_component->route;
                    i32 nearest_next_checkpoint_index = 0;
                    f32 nearest_next_checkpoint_distance = glm::distance(glm::vec2(current_position.x, current_position.z), route.at(nearest_next_checkpoint_index));
                    
                    i32 index = 0;
                    for (auto route_it : route)
                    {
                        f32 distance = glm::distance(glm::vec2(current_position.x, current_position.z), route_it);
                        if (distance < nearest_next_checkpoint_distance)
                        {
                            nearest_next_checkpoint_distance = distance;
                            nearest_next_checkpoint_index = index;
                        }
                        index++;
                    }
                    
                    glm::vec3 direction_arrow_rotation = direction_arrow->rotation;
                    nearest_next_checkpoint_index = (nearest_next_checkpoint_index + 2) % route.size();
                    glm::vec2 direction = glm::normalize(glm::vec2(current_position.x, current_position.z) - route.at(nearest_next_checkpoint_index));
                    f32 goal_rotation = atan2f(direction.x, -direction.y);
                    goal_rotation = glm::degrees(goal_rotation) + glm::degrees(current_rotation.y);
                    direction_arrow_rotation.z = glm::mix_angles_degrees(direction_arrow_rotation.z, goal_rotation, .1f);
                    direction_arrow->rotation = direction_arrow_rotation;
                }
            }
            m_all_characters[character_key] = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_ui_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            auto ui_interaction_component = entity->get_component<ces_ui_interaction_component>();
            switch (ui_interaction_component->get_ui())
            {
                case ces_ui_interaction_component::e_ui_open_levels_list_dialog_button:
                {
                    m_open_levels_list_dialog_button = entity;
                    if(!m_open_levels_list_dialog_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_open_levels_list_dialog_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            if (!m_current_pushed_dialog.expired() && m_current_pushed_dialog.lock() == m_levels_list_dialog.lock())
                            {
                                pop_current_dialog();
                            }
                            else
                            {
                                push_levels_list_dialog();
                            }
                        });
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_open_garage_button:
                {
                    m_open_garage_button = entity;
                    if(!m_open_garage_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_open_garage_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            pop_current_dialog();
                            if (m_scene.lock()->get_component<ces_scene_state_automat_component>()->mode == ces_scene_state_automat_component::e_mode_main_menu)
                            {
                                m_scene.lock()->get_component<ces_scene_state_automat_component>()->mode = ces_scene_state_automat_component::e_mode_garage;
                                m_scene.lock()->get_component<ces_scene_state_automat_component>()->state = ces_scene_state_automat_component::e_state_should_preload;
                            }
                        });
                    }
                }
                    break;
                case ces_ui_interaction_component::e_ui_back_from_garage_button:
                {
                    m_back_from_garage_button = entity;
                    if(!m_back_from_garage_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_back_from_garage_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            pop_current_dialog();
                            if (m_scene.lock()->get_component<ces_scene_state_automat_component>()->mode == ces_scene_state_automat_component::e_mode_garage)
                            {
                                m_scene.lock()->get_component<ces_scene_state_automat_component>()->mode = ces_scene_state_automat_component::e_mode_main_menu;
                                m_scene.lock()->get_component<ces_scene_state_automat_component>()->state = ces_scene_state_automat_component::e_state_should_preload;
                            }
                        });
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_next_car_in_garage_button:
                {
                    m_next_car_in_garage_button = entity;
                    if(!m_next_car_in_garage_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_next_car_in_garage_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            pop_current_dialog();
                            if (!m_camera_follow_car.expired())
                            {
                                const auto main_car = std::static_pointer_cast<gb::game_object_3d>(m_camera_follow_car.lock());
                                const auto scene_fabricator_component = root->get_component<ces_scene_fabricator_component>();
                                const gameplay_fabricator_shared_ptr gameplay_fabricator = scene_fabricator_component->gameplay_fabricator;
                                
                                static i32 index = 0;
                                index++;
                                std::stringstream car_index_str;
                                car_index_str<<"car_0"<<index % 2 + 1;
                                gameplay_fabricator->reconstruct_car_geometry(main_car, car_index_str.str());
                            }
                        });
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_prev_car_in_garage_button:
                {
                    m_prev_car_in_garage_button = entity;
                    if(!m_prev_car_in_garage_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_prev_car_in_garage_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            pop_current_dialog();
                            if (!m_camera_follow_car.expired())
                            {
                                m_camera_follow_car.lock()->remove_from_parent();
                                m_camera_follow_car.reset();
                            }
                        });
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_scores_label:
                {
                    m_scores_label = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_countdown_label:
                {
                    m_countdown_label = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui_levels_list_dialog:
                {
                    m_levels_list_dialog = entity;
                    /*auto close_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(entity)->get_control(ces_ui_interaction_component::k_questlog_dialog_close_button));
                    if(!close_button->is_pressed_callback_exist())
                    {
                        close_button->set_on_pressed_callback([this](const gb::ces_entity_shared_ptr&) {
                            m_questlog_dialog.lock()->visible = false;
                        });
                    }*/
                }
                    break;
                    
                default:
                    break;
                    
                /*case ces_ui_interaction_component::e_type_quest_dialog:
                {
                    m_quests_dialog = entity;
                    const auto& character_selector_component = m_main_character.lock()->get_component<ces_character_selector_component>();
                    auto close_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(entity)->get_control(ces_ui_interaction_component::k_quests_dialog_close_button));
                    if(!close_button->is_pressed_callback_exist())
                    {
                        close_button->set_on_pressed_callback([this, character_selector_component](const gb::ces_entity_shared_ptr&){
                            m_quests_dialog.lock()->visible = false;
                        });
                    }

                    if(!character_selector_component->is_selections_exist())
                    {
                        entity->visible = false;
                    }
                    else
                    {
                        auto selected_opponent = character_selector_component->get_selections().at(0);
                        auto character_statistic_component = selected_opponent.lock()->get_component<ces_character_statistic_component>();
                        if(character_statistic_component->mode != ces_character_statistic_component::e_mode_npc)
                        {
                            entity->visible = false;
                        }
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_type_action_console:
                {
                    m_action_console = entity;
                }
                    break;
                
                case ces_ui_interaction_component::e_type_ability_button:
                {
                    m_abilities_buttons[entity->tag] = entity;
                    ces_ui_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_pressed);
                    ces_ui_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_released);
                }
                    break;
                case ces_ui_interaction_component::e_type_character_avatar_icon:
                {
                    m_character_avatar_icon = entity;
                    auto ui_avatar_icon_component = entity->get_component<ces_ui_avatar_icon_component>();
                    if(!ui_avatar_icon_component->get_avatar_2d_entity() &&
                       !ui_avatar_icon_component->get_avatar_3d_entity())
                    {
                        ui_avatar_icon_component->set_avatar_2d_entity(m_main_character.lock()->get_component<ces_ui_avatar_icon_component>()->get_avatar_2d_entity());
                        ui_avatar_icon_component->set_avatar_3d_entity(m_main_character.lock()->get_component<ces_ui_avatar_icon_component>()->get_avatar_3d_entity());
                        entity->add_child(ui_avatar_icon_component->get_avatar_3d_entity());
                        entity->add_child(ui_avatar_icon_component->get_avatar_2d_entity());
                    }
                }
                    break;
                case ces_ui_interaction_component::e_type_opponent_avatar_icon:
                {
                    m_opponent_avatar_icon = entity;
                    auto character_selector_component = m_main_character.lock()->get_component<ces_character_selector_component>();
                    auto current_opponent_ui_avatar_icon_component = entity->get_component<ces_ui_avatar_icon_component>();
                    if(character_selector_component->is_selections_exist())
                    {
                        auto selected_opponent = character_selector_component->get_selections().at(0);
                        auto selected_opponent_ui_avatar_icon_component = selected_opponent.lock()->get_component<ces_ui_avatar_icon_component>();
                        if(selected_opponent_ui_avatar_icon_component->get_avatar_2d_entity() !=
                           current_opponent_ui_avatar_icon_component->get_avatar_2d_entity())
                        {
                            if(current_opponent_ui_avatar_icon_component->get_avatar_2d_entity() != nullptr)
                            {
                                entity->remove_child(current_opponent_ui_avatar_icon_component->get_avatar_2d_entity());
                            }
                            if(current_opponent_ui_avatar_icon_component->get_avatar_3d_entity() != nullptr)
                            {
                                entity->remove_child(current_opponent_ui_avatar_icon_component->get_avatar_3d_entity());
                            }
                            
                            current_opponent_ui_avatar_icon_component->set_avatar_2d_entity(selected_opponent_ui_avatar_icon_component->get_avatar_2d_entity());
                            current_opponent_ui_avatar_icon_component->set_avatar_3d_entity(selected_opponent_ui_avatar_icon_component->get_avatar_3d_entity());
                            entity->add_child(current_opponent_ui_avatar_icon_component->get_avatar_3d_entity());
                            entity->add_child(current_opponent_ui_avatar_icon_component->get_avatar_2d_entity());
                        }
                        entity->visible = true;
                    }
                    else
                    {
                        if(current_opponent_ui_avatar_icon_component->get_avatar_2d_entity() != nullptr)
                        {
                            entity->remove_child(current_opponent_ui_avatar_icon_component->get_avatar_2d_entity());
                            current_opponent_ui_avatar_icon_component->set_avatar_2d_entity(nullptr);
                        }
                        if(current_opponent_ui_avatar_icon_component->get_avatar_3d_entity() != nullptr)
                        {
                            entity->remove_child(current_opponent_ui_avatar_icon_component->get_avatar_3d_entity());
                            current_opponent_ui_avatar_icon_component->set_avatar_3d_entity(nullptr);
                        }
                        entity->visible = false;
                    }
                }
                    break;
                    
                default:
                    break;*/
            }
        });
    }
    
    void ces_ui_interaction_system::on_feed_end(f32 dt)
    {
        
    }
    
    void ces_ui_interaction_system::add_touch_recognition(const gb::ces_entity_shared_ptr& entity,
                                                       gb::e_input_state input_state)
    {
        const auto& bound_touch_component = entity->get_component<gb::ces_bound_touch_component>();
        if(!bound_touch_component->is_respond_to(input_state, gb::e_input_source::e_input_source_mouse_left))
        {
            bound_touch_component->enable(input_state, gb::e_input_source_mouse_left, true);
            bound_touch_component->add_callback(input_state, std::bind(&ces_ui_interaction_system::on_touched, this,
                                                                       std::placeholders::_1,
                                                                       std::placeholders::_2,
                                                                       std::placeholders::_3,
                                                                       std::placeholders::_4));
        }
    }
    
    void ces_ui_interaction_system::on_touched(const gb::ces_entity_shared_ptr& entity, const glm::vec2& touch_point,
                                            gb::e_input_source input_source, gb::e_input_state input_state)
    {
        /*if(entity == m_attack_button.lock() && input_state == gb::e_input_state_released)
        {
            auto current_character = m_main_character.lock();
            auto character_selector_component = current_character->get_component<ces_character_selector_component>();
            if(character_selector_component->is_selections_exist())
            {
                auto opponent_character = character_selector_component->get_selections().at(0).lock();
                const auto& character_statistic_component = current_character->get_component<ces_character_statistic_component>();
                const auto& current_character_transformation_component = current_character->get_component<gb::ces_transformation_2d_component>();
                const auto& opponent_character_transformation_component = opponent_character->get_component<gb::ces_transformation_2d_component>();
                glm::vec2 opponent_character_position = opponent_character_transformation_component->get_position();
                glm::vec2 current_character_position = current_character_transformation_component->get_position();
                f32 distance = glm::distance(current_character_position, opponent_character_position);
                f32 attack_distance = character_statistic_component->current_attack_distance;
                if(distance <= attack_distance)
                {
                    const auto& current_character_parts_component = current_character->get_component<ces_character_parts_component>();
                    auto light_source_entity = current_character_parts_component->get_light_source_part();
                    auto light_source_mesh = light_source_entity->get_component<gb::ces_light_mask_component>()->get_mesh();
                    
                    const auto& opponent_character_parts_component = opponent_character->get_component<ces_character_parts_component>();
                    auto bounds_entity = opponent_character_parts_component->get_bounds_part();
                    auto bounds_mesh = bounds_entity->get_component<gb::ces_geometry_component>()->get_mesh();
                    
                    if(gb::mesh_2d::intersect(bounds_mesh->get_vbo(),
                                              bounds_mesh->get_ibo(),
                                              opponent_character_transformation_component->get_matrix_m(),
                                              true,
                                              light_source_mesh->get_vbo(),
                                              light_source_mesh->get_ibo(),
                                              glm::mat4(1.f),
                                              false,
                                              nullptr,
                                              nullptr))
                    {
                        const auto& character_state_automat_component = current_character->get_component<ces_scene_state_automat_component>();
                        auto actions_processor = character_state_automat_component->get_actions_processor();
                        actions_processor->interrupt_all_actions();
                        auto character_statistic_component = opponent_character->get_component<ces_character_statistic_component>();
                        if(character_statistic_component->mode != ces_character_statistic_component::e_mode_npc)
                        {
                            auto attack_action = std::make_shared<ai_attack_action>(current_character);
                            attack_action->set_parameters(std::static_pointer_cast<gb::game_object_2d>(opponent_character));
                            attack_action->set_start_callback([](const ai_action_shared_ptr& action) {
                                auto character = action->get_owner();
                                const auto& character_state_automat_component = character->get_component<ces_scene_state_automat_component>();
                                character_state_automat_component->set_state(game::ces_scene_state_automat_component::e_state_attack);
                            });
                            attack_action->set_end_callback([](const ai_action_shared_ptr& action) {
                                auto character = action->get_owner();
                                const auto& character_state_automat_component = character->get_component<ces_scene_state_automat_component>();
                                character_state_automat_component->set_state(game::ces_scene_state_automat_component::e_state_idle);
                            });
                            actions_processor->add_action(attack_action);
                        }
                        else
                        {
                            character_state_automat_component->set_state(game::ces_scene_state_automat_component::e_state_idle);
                            const auto& quest_giver_component = opponent_character->get_component<ces_quest_giver_component>();
                            if(quest_giver_component->is_quests_exist())
                            {
                                auto quest_receiver_component = current_character->get_component<ces_quest_receiver_component>();
                                const auto& npc_quests = quest_giver_component->get_all_quests_ids();
                                bool is_available_quest_exist = false;
                                for(auto it : npc_quests)
                                {
                                    if(!quest_receiver_component->is_quest_exist(it))
                                    {
                                        is_available_quest_exist = true;
                                        auto ui_quest_dialog_component = m_quests_dialog.lock()->get_component<ces_ui_quest_dialog_component>();
                                        ui_quest_dialog_component->set_selected_quest_id(it);
                                        break;
                                    }
                                }
                                ces_ui_interaction_system::show_quests_dialog();
                            }
                        }
                    }
                }
                else
                {
                    /*std::static_pointer_cast<gb::ui::action_console>(m_action_console.lock())->write("I need to be closer");

                    auto character_state_automat_component = current_character->get_component<ces_character_state_automat_component>();
                    auto actions_processor = character_state_automat_component->get_actions_processor();
                    actions_processor->interrupt_all_actions();
                   
                    auto character_pathfinder_component = current_character->get_component<ces_character_pathfinder_component>();
                    auto pathfinder = character_pathfinder_component->get_pathfinder();
                    
                    auto level = m_level.lock();
                    auto level_path_grid_component = level->get_component<ces_level_path_grid_component>();
                    auto path_grid = level_path_grid_component->get_path_grid();
                    
                    ai_chase_action_shared_ptr chase_action = std::make_shared<ai_chase_action>(current_character);
                    chase_action->set_parameters(std::static_pointer_cast<gb::game_object_2d>(opponent_character),
                                                 path_grid);
                    chase_action->set_start_callback([](const ai_action_shared_ptr& action) {
                        auto character = action->get_owner();
                        const auto& character_state_automat_component = character->get_component<ces_character_state_automat_component>();
                        character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_chase);
                    });
                    chase_action->set_end_callback([this](const ai_action_shared_ptr& action) {
                        ces_ui_interaction_system::on_touched(m_attack_button.lock(), glm::vec2(0.f), gb::e_input_source_mouse_left, gb::e_input_state_released);
                    });
                    actions_processor->add_action(chase_action);
                }
            }
            else
            {
                std::static_pointer_cast<gb::ui::action_console>(m_action_console.lock())->write("I need a target");
            }
        }*/
    }
    
    void ces_ui_interaction_system::pop_current_dialog()
    {
        if (!m_current_pushed_dialog.expired())
        {
            m_current_pushed_dialog.lock()->visible = false;
            m_current_pushed_dialog.reset();
        }
    }
    
    void ces_ui_interaction_system::push_levels_list_dialog()
    {
        m_levels_list_dialog.lock()->visible = true;
        m_current_pushed_dialog = m_levels_list_dialog.lock();
        
        const auto levels_list_table_view = std::static_pointer_cast<gb::ui::table_view>(std::static_pointer_cast<gb::ui::dialog>(m_levels_list_dialog.lock())->get_control(ces_ui_interaction_component::k_levels_list_dialog_levels_table));
        
        std::vector<gb::ui::table_view_cell_data_shared_ptr> data;
        for (i32 i = 0; i < 150; ++i)
        {
            auto level_data = std::make_shared<ui::levels_list_table_view_cell_data>();
            level_data->id = i;
            data.push_back(level_data);
        }
        levels_list_table_view->set_data_source(data);
        levels_list_table_view->set_on_get_cell_callback([=](i32 index, const gb::ui::table_view_cell_data_shared_ptr& data, const gb::ces_entity_shared_ptr& table_view) {
            gb::ui::table_view_cell_shared_ptr cell = nullptr;
            cell = std::static_pointer_cast<gb::ui::table_view>(table_view)->reuse_cell("level_cell", index);
            if(!cell)
            {
                cell = gb::ces_entity::construct<ui::levels_list_table_view_cell>(std::static_pointer_cast<gb::ui::table_view>(table_view)->get_fabricator(),
                                                                                                       index, "level_cell");
                cell->create();
                cell->size = glm::vec2(192.f, 92.f);
                
                std::static_pointer_cast<ui::levels_list_table_view_cell>(cell)->set_start_level_button_callback_t([=](i32 index) {
                    auto data_source = std::static_pointer_cast<gb::ui::table_view>(table_view)->get_data_source();
                    auto data = data_source.at(index);
                    
                    m_scene.lock()->get_component<ces_scene_state_automat_component>()->mode = ces_scene_state_automat_component::e_mode_in_game;
                    m_scene.lock()->get_component<ces_scene_state_automat_component>()->state = ces_scene_state_automat_component::e_state_should_preload;
                    pop_current_dialog();
                });
            }
    
            return cell;
        });
        levels_list_table_view->set_on_get_table_cell_height_callback([](i32 index) {
            return 96.f;
        });
        levels_list_table_view->reload_data();
        
        /*const auto& character_selector_component = m_main_character.lock()->get_component<ces_character_selector_component>();
        auto opponent_character = character_selector_component->get_selections().at(0).lock();
        const auto& quest_giver_component = opponent_character->get_component<ces_quest_giver_component>();
        const auto& quest_receiver_component = m_main_character.lock()->get_component<ces_quest_receiver_component>();
        if(quest_giver_component->is_quests_exist())
        {
            auto quests_table_view = std::static_pointer_cast<gb::ui::table_view>(std::static_pointer_cast<gb::ui::dialog>(m_quests_dialog.lock())->get_control(ces_ui_interaction_component::k_quests_dialog_quests_table));
            const auto& quests = quest_giver_component->get_all_quests_ids();
            std::vector<gb::ui::table_view_cell_data_shared_ptr> data;
            for(const auto& quest : quests)
            {
                auto quest_data = std::make_shared<ces_ui_quest_dialog_component::quest_table_view_cell_data>();
                quest_data->set_quest_id(quest);
                data.push_back(quest_data);
            }
            quests_table_view->set_data_source(data);
            quests_table_view->set_on_get_cell_callback([this, quest_receiver_component, quest_giver_component](i32 index, const gb::ui::table_view_cell_data_shared_ptr& data, const gb::ces_entity_shared_ptr& table_view) {
                gb::ui::table_view_cell_shared_ptr cell = nullptr;
                cell = std::static_pointer_cast<gb::ui::table_view>(table_view)->reuse_cell("quest_cell", index);
                if(!cell)
                {
                    cell = gb::ces_entity::construct<ces_ui_quest_dialog_component::quest_table_view_cell>(std::static_pointer_cast<gb::ui::table_view>(table_view)->get_fabricator(),
                                                                                                           index, "quest_cell");
                    cell->create();
                    cell->size = glm::vec2(384.f, 80.f);
                    cell->set_background_color(glm::u8vec4(192, 192, 192, 255));
                    
                    std::static_pointer_cast<ces_ui_quest_dialog_component::quest_table_view_cell>(cell)->set_accept_quest_button_callback([this, table_view, quest_receiver_component, quest_giver_component](i32 index) {
                        auto data_source = std::static_pointer_cast<gb::ui::table_view>(table_view)->get_data_source();
                        auto data = data_source.at(index);
                        quest_receiver_component->add_to_questlog(std::static_pointer_cast<ces_ui_quest_dialog_component::quest_table_view_cell_data>(data)->get_quest_id(),
                                                                  quest_giver_component->get_quest(std::static_pointer_cast<ces_ui_quest_dialog_component::quest_table_view_cell_data>(data)->get_quest_id()));
                        //ces_ui_interaction_system::show_quests_dialog();
                    });
                }
                std::static_pointer_cast<ces_ui_quest_dialog_component::quest_table_view_cell>(cell)->set_quest_in_progress(quest_receiver_component->is_quest_exist(std::static_pointer_cast<ces_ui_quest_dialog_component::quest_table_view_cell_data>(data)->get_quest_id()));
                
                return cell;
            });
            quests_table_view->set_on_get_table_cell_height_callback([](i32 index) {
                return 96.f;
            });
            quests_table_view->reload_data();
        }*/
    }
    
    void ces_ui_interaction_system::show_questlog_dialog()
    {
        /*m_questlog_dialog.lock()->visible = true;
        auto quests_table_view = std::static_pointer_cast<gb::ui::table_view>(std::static_pointer_cast<gb::ui::dialog>(m_questlog_dialog.lock())->get_control(ces_ui_interaction_component::k_questlog_dialog_quests_table));
        auto no_quests_label = std::static_pointer_cast<gb::ui::dialog>(m_questlog_dialog.lock())->get_control(ces_ui_interaction_component::k_questlog_dialog_no_quests_label);
        const auto& quest_receiver_component = m_main_character.lock()->get_component<ces_quest_receiver_component>();
        const auto& quests = quest_receiver_component->get_all_quests();
        if(quests.size() != 0)
        {
            quests_table_view->visible = true;
            no_quests_label->visible = false;
            std::vector<gb::ui::table_view_cell_data_shared_ptr> data;
            for(const auto& quest : quests)
            {
                auto quest_data = std::make_shared<ces_ui_questlog_dialog_component::quest_table_view_cell_data>();
                quest_data->set_quest_id(quest.first);
                data.push_back(quest_data);
            }
            quests_table_view->set_data_source(data);
            quests_table_view->set_on_get_cell_callback([this](i32 index, const gb::ui::table_view_cell_data_shared_ptr& data, const gb::ces_entity_shared_ptr& table_view) {
                gb::ui::table_view_cell_shared_ptr cell = nullptr;
                cell = std::static_pointer_cast<gb::ui::table_view>(table_view)->reuse_cell("quest_cell", index);
                if(!cell)
                {
                    cell = gb::ces_entity::construct<ces_ui_questlog_dialog_component::quest_table_view_cell>(std::static_pointer_cast<gb::ui::table_view>(table_view)->get_fabricator(),
                                                                                                              index, "quest_cell");
                    cell->create();
                    cell->size = glm::vec2(512.f, 80.f);
                    cell->set_background_color(glm::u8vec4(192, 192, 192, 255));
                    
                    std::static_pointer_cast<ces_ui_questlog_dialog_component::quest_table_view_cell>(cell)->set_track_quest_button_callback([table_view](i32 index) {
                        auto data_source = std::static_pointer_cast<gb::ui::table_view>(table_view)->get_data_source();
                        auto data = data_source.at(index);
                    });
                    
                    std::static_pointer_cast<ces_ui_questlog_dialog_component::quest_table_view_cell>(cell)->set_remove_quest_button_callback([this, table_view](i32 index) {
                        auto data_source = std::static_pointer_cast<gb::ui::table_view>(table_view)->get_data_source();
                        auto data = data_source.at(index);
                        const auto& quest_receiver_component = m_main_character.lock()->get_component<ces_quest_receiver_component>();
                        quest_receiver_component->remove_from_questlog(std::static_pointer_cast<ces_ui_questlog_dialog_component::quest_table_view_cell_data>(data)->get_quest_id());
                        ces_ui_interaction_system::show_questlog_dialog();
                    });
                }
                return cell;
            });
            quests_table_view->set_on_get_table_cell_height_callback([](i32 index) {
                return 96.f;
            });
            quests_table_view->reload_data();
        }
        else
        {
            quests_table_view->visible = false;
            no_quests_label->visible = true;
        }*/
    }
    
    void ces_ui_interaction_system::on_dragging(const gb::ces_entity_shared_ptr& entity, const glm::vec2& delta)
    {
        const auto main_character = std::static_pointer_cast<gb::game_object_3d>(m_main_character.lock());
        const auto character_navigation_component = main_character->get_component<ces_character_navigation_component>();
        
        character_navigation_component->stop_move();
        character_navigation_component->stop_steer();
        
        const auto car_input_component = main_character->get_component<ces_car_input_component>();
        const auto car_model_component = main_character->get_component<ces_car_model_component>();
        
        car_input_component->throttle = 0.f;
        car_input_component->steer_angle = 0.f;
        car_input_component->brake = 0.f;
        
        if (delta.x > 0.f &&
            delta.y > 0.f)
        {
            character_navigation_component->move_forward();
            character_navigation_component->steer_left();
            
            car_input_component->updated = true;
            car_input_component->throttle = car_model_component->get_max_force();
            car_input_component->steer_angle = 30.f;
        }
        else if (delta.x > 0.f &&
                 delta.y < 0.f)
        {
            character_navigation_component->move_backward();
            character_navigation_component->steer_right();
        }
        else if (delta.x < 0.f &&
                 delta.y > 0.f)
        {
            character_navigation_component->move_forward();
            character_navigation_component->steer_right();
            
            car_input_component->updated = true;
            car_input_component->throttle = car_model_component->get_max_force();
            car_input_component->steer_angle = -30.f;
        }
        else if (delta.x < 0.f &&
                 delta.y < 0.f)
        {
            character_navigation_component->move_backward();
            character_navigation_component->steer_left();
        }
        else if (delta.x > 0.f)
        {
            character_navigation_component->steer_left();
        }
        else if (delta.x < 0.f)
        {
            character_navigation_component->steer_right();
        }
        else if (delta.y > 0.f)
        {
            character_navigation_component->move_forward();
            
            car_input_component->updated = true;
            car_input_component->throttle = car_model_component->get_max_force();
        }
        else if (delta.y < 0.f)
        {
            character_navigation_component->move_backward();
        }
    }
    
    void ces_ui_interaction_system::on_drag_ended(const gb::ces_entity_shared_ptr& entity, const glm::vec2& point)
    {
        const auto main_character = std::static_pointer_cast<gb::game_object_3d>(m_main_character.lock());
        const auto character_navigation_component = main_character->get_component<ces_character_navigation_component>();
        
        character_navigation_component->stop_move();
        character_navigation_component->stop_steer();
        
        const auto car_input_component = main_character->get_component<ces_car_input_component>();
        const auto car_model_component = main_character->get_component<ces_car_model_component>();
        
        car_input_component->throttle = 0.f;
        car_input_component->steer_angle = 0.f;
        car_input_component->brake = 200.f;
    }
}
