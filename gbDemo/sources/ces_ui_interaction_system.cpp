//
//  ces_ui_interaction_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/27/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ui_interaction_system.h"
#include "ces_scene_state_automat_component.h"
#include "ces_car_statistic_component.h"
#include "ces_car_parts_component.h"
#include "ces_ui_interaction_component.h"
#include "ces_bound_touch_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_light_mask_component.h"
#include "ces_geometry_component.h"
#include "shape_3d.h"
#include "label_3d.h"
#include "dialog.h"
#include "button.h"
#include "textfield.h"
#include "joystick.h"
#include "image_button.h"
#include "table_view.h"
#include "table_view_cell.h"
#include "action_console.h"
#include "game_object_2d.h"
#include "camera_3d.h"
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
#include "cars_list_table_view_cell.h"
#include "ces_scene_state_automat_component.h"
#include "ces_scene_fabricator_component.h"
#include "ces_car_ai_input_component.h"
#include "gameplay_fabricator.h"
#include "gameplay_ui_fabricator.h"
#include "game_objects_custom_uniforms.h"
#include "ces_garage_database_component.h"
#include "ces_levels_database_component.h"
#include "ui_controls_helper.h"
#include "progress_bar.h"
#include "sprite.h"

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
        
        ces_base_system::add_required_component_guid(m_car_components_mask, ces_car_statistic_component::class_guid());
        ces_base_system::add_required_components_mask(m_car_components_mask);
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
                    if (!m_main_car.expired())
                    {
                        const auto car = std::static_pointer_cast<gb::game_object_3d>(m_main_car.lock());
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
                            f32 current_round_time = std::max(round_time - delta, 0.f);
                            if (current_round_time > 0.f)
                            {
                                level_descriptor_component->current_round_time = current_round_time;
                            }
                            else
                            {
                                level_descriptor_component->current_round_time = 0.f;
                                const auto level_descriptor_component = m_level.lock()->get_component<ces_level_descriptor_component>();
                                level_descriptor_component->is_finished = true;
                                
                                const auto car_input_component = m_main_car.lock()->get_component<ces_car_input_component>();
                                if (car_input_component)
                                {
                                    m_main_car.lock()->remove_component(car_input_component);
                                    const auto car_ai_input_component = std::make_shared<ces_car_ai_input_component>();
                                    m_main_car.lock()->add_component(car_ai_input_component);
                                    pop_current_dialog();
                                    push_win_dialog(root);
                                }
                                
                                if (!m_countdown_label.expired())
                                {
                                    m_countdown_label.lock()->remove_from_parent();
                                    m_countdown_label.reset();
                                }
                                
                                if (!m_cars_list_dialog.expired())
                                {
                                    m_cars_list_dialog.lock()->remove_from_parent();
                                    m_cars_list_dialog.reset();
                                }
                            }
                            
                            if (!m_countdown_label.expired())
                            {
                                const auto countdown_label = std::static_pointer_cast<gb::ui::textfield>(m_countdown_label.lock());
                                countdown_label->visible = true;
                                i32 seconds = current_round_time;
                                f32 f_milliseconds = current_round_time - seconds;
                                i32 milliseconds = f_milliseconds * 10;
                                    
                                std::stringstream round_time_value_string_stream;
                                round_time_value_string_stream<<(seconds < 10 ? "0" : "")<<seconds<<":"<<milliseconds<<"0 sec";
                                    
                                std::string round_time_text = "TIME LEFT: ";
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
                const auto car_parts_component = car->get_component<ces_car_parts_component>();
                
                glm::vec3 current_position = car->position;
                glm::vec3 current_rotation = car->rotation;
                
                glm::vec2 velocity_wc = car_descriptor_component->velocity_wc;
                f32 velocity_wc_length = glm::length(velocity_wc);
                f32 current_velocity_length_squared = velocity_wc_length * velocity_wc_length;
                f32 max_speed_squared = car_model_component->get_max_speed() * car_model_component->get_max_speed();
                f32 current_speed_factor = glm::clamp(current_velocity_length_squared / max_speed_squared, 0.f, 1.f);
                
                if (car_camera_follow_component->is_preview_mode == false)
                {
                    current_position.x += velocity_wc.x * current_speed_factor * .2f;
                    current_position.z += velocity_wc.y * current_speed_factor * .2f;
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
        
        ces_base_system::enumerate_entities_with_components(m_car_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            std::string character_key = entity->tag;
            auto car_statistic_component = entity->get_component<ces_car_statistic_component>();
            if(car_statistic_component->mode == ces_car_statistic_component::e_mode::e_mode_player)
            {
                m_main_car = entity;
                
                const auto main_car = std::static_pointer_cast<gb::game_object_3d>(m_main_car.lock());
                const auto car_descriptor_component = main_car->get_component<ces_car_descriptor_component>();
                const auto car_model_component = main_car->get_component<ces_car_model_component>();
                const auto car_parts_component = main_car->get_component<ces_car_parts_component>();
                const auto car_drift_state_component = main_car->get_component<ces_car_drift_state_component>();
                const auto car_gear_component = main_car->get_component<ces_car_gear_component>();
               
                const auto speed_value_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_speed_value_label));
                const auto rpm_value_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_rpm_value_label));
                const auto drift_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_label));
                const auto drift_value_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_drift_value_label));
                
                const auto direction_arrow = std::static_pointer_cast<gb::shape_3d>(car_parts_component->get_part(ces_car_parts_component::parts::k_ui_direction_arrow));
                
                glm::vec3 current_position = main_car->position;
                glm::vec3 current_rotation = main_car->rotation;
                
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
                
                const auto level_descriptor_component = m_level.lock()->get_component<ces_level_descriptor_component>();
                if (!level_descriptor_component->is_finished)
                {
                    const auto damage_bar = ui_controls_helper::get_control_as<gb::ui::progress_bar>(ces_ui_interaction_component::e_ui::e_ui_car_damage_bar);
                    if (damage_bar)
                    {
                        f32 max_damage = car_descriptor_component->max_damage;
                        f32 current_damage = car_descriptor_component->current_damage;
                        if (current_damage <= max_damage)
                        {
                            damage_bar->set_progress(std::max(.05f, current_damage / max_damage));
                            if (current_damage >= max_damage * .33)
                            {
                                damage_bar->set_progress_line_color(glm::u8vec4(255, 255, 64, 255));
                            }
                            if (current_damage > max_damage * .66)
                            {
                                damage_bar->set_progress_line_color(glm::u8vec4(255, 64, 64, 255));
                            }
                        }
                    }
                }
            }
            else
            {
                m_ai_cars[character_key] = entity;
            }
            m_all_cars[character_key] = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_ui_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            auto ui_interaction_component = entity->get_component<ces_ui_interaction_component>();
            switch (ui_interaction_component->get_ui())
            {
                case ces_ui_interaction_component::e_ui::e_ui_open_levels_list_dialog_button:
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
                                push_levels_list_dialog(root);
                            }
                        });
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_open_garage_button:
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
                case ces_ui_interaction_component::e_ui::e_ui_back_from_garage_button:
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
                    
                case ces_ui_interaction_component::e_ui::e_ui_next_car_in_garage_button:
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
                                
                                if (!m_level.expired())
                                {
                                    const auto garage_database_component = root->get_component<ces_garage_database_component>();
                                    auto selected_car_id = garage_database_component->get_previewed_car_id();
                                    selected_car_id++;
                                    if (selected_car_id > garage_database_component->get_max_cars_count())
                                    {
                                        selected_car_id = 1;
                                    }
                                    garage_database_component->set_previewed_car_id(selected_car_id);
                                    std::stringstream car_configuration_filename;
                                    car_configuration_filename<<"car_0"<<selected_car_id;
                                    gameplay_fabricator->reconstruct_car_geometry(main_car, car_configuration_filename.str());
                                    
                                    bool is_car_oppenned = garage_database_component->is_car_oppenned(1, selected_car_id);
                                    
                                    if (!m_select_car_button.expired())
                                    {
                                        m_select_car_button.lock()->visible = is_car_oppenned;
                                        if (selected_car_id == garage_database_component->get_selected_car(1)->get_id())
                                        {
                                            m_select_car_button.lock()->as<gb::ui::image_button>()->set_image_color(glm::u8vec4(64, 64, 255, 255));
                                        }
                                        else
                                        {
                                            m_select_car_button.lock()->as<gb::ui::image_button>()->set_image_color(glm::u8vec4(255, 255, 255, 255));
                                        }
                                    }
                                    
                                    if (!m_unlock_car_button.expired())
                                    {
                                        m_unlock_car_button.lock()->visible = !is_car_oppenned;
                                    }
                                }
                            }
                        });
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_prev_car_in_garage_button:
                {
                    m_prev_car_in_garage_button = entity;
                    if(!m_prev_car_in_garage_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_prev_car_in_garage_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            pop_current_dialog();
                            if (!m_camera_follow_car.expired())
                            {
                                const auto main_car = std::static_pointer_cast<gb::game_object_3d>(m_camera_follow_car.lock());
                                const auto scene_fabricator_component = root->get_component<ces_scene_fabricator_component>();
                                const gameplay_fabricator_shared_ptr gameplay_fabricator = scene_fabricator_component->gameplay_fabricator;
                                
                                if (!m_level.expired())
                                {
                                    const auto garage_database_component = root->get_component<ces_garage_database_component>();
                                    auto selected_car_id = garage_database_component->get_previewed_car_id();
                                    selected_car_id--;
                                    if (selected_car_id < 1)
                                    {
                                        selected_car_id = garage_database_component->get_max_cars_count();
                                    }
                                    garage_database_component->set_previewed_car_id(selected_car_id);
                                    std::stringstream car_configuration_filename;
                                    car_configuration_filename<<"car_0"<<selected_car_id;
                                    gameplay_fabricator->reconstruct_car_geometry(main_car, car_configuration_filename.str());
                                    
                                    bool is_car_oppenned = garage_database_component->is_car_oppenned(1, selected_car_id);
                                    
                                    if (!m_select_car_button.expired())
                                    {
                                        m_select_car_button.lock()->visible = is_car_oppenned;
                                        if (selected_car_id == garage_database_component->get_selected_car(1)->get_id())
                                        {
                                            m_select_car_button.lock()->as<gb::ui::image_button>()->set_image_color(glm::u8vec4(64, 64, 255, 255));
                                        }
                                        else
                                        {
                                            m_select_car_button.lock()->as<gb::ui::image_button>()->set_image_color(glm::u8vec4(255, 255, 255, 255));
                                        }
                                    }
                                    
                                    if (!m_unlock_car_button.expired())
                                    {
                                        m_unlock_car_button.lock()->visible = !is_car_oppenned;
                                    }
                                }
                            }
                        });
                    }
                }
                    break;
                
                case ces_ui_interaction_component::e_ui::e_ui_car_skin_1_button:
                {
                    m_car_skin_1_button = entity;
                    if(!m_car_skin_1_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_car_skin_1_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            pop_current_dialog();
                            const auto main_car = std::static_pointer_cast<gb::game_object_3d>(m_camera_follow_car.lock());
                            const auto scene_fabricator_component = root->get_component<ces_scene_fabricator_component>();
                            const gameplay_fabricator_shared_ptr gameplay_fabricator = scene_fabricator_component->gameplay_fabricator;
                            if (!m_level.expired())
                            {
                                const auto garage_database_component = root->get_component<ces_garage_database_component>();
                                auto selected_car_id = garage_database_component->get_previewed_car_id();
                                std::stringstream car_configuration_filename;
                                car_configuration_filename<<"car_0"<<selected_car_id;
                                gameplay_fabricator->reskin_car(main_car, car_configuration_filename.str(), 1);
                                garage_database_component->select_car_skin(1, selected_car_id, 1);
                            }
                        });
                    }
                }
                break;
                
                case ces_ui_interaction_component::e_ui::e_ui_car_skin_2_button:
                {
                    m_car_skin_2_button = entity;
                    if(!m_car_skin_2_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_car_skin_2_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            pop_current_dialog();
                            const auto main_car = std::static_pointer_cast<gb::game_object_3d>(m_camera_follow_car.lock());
                            const auto scene_fabricator_component = root->get_component<ces_scene_fabricator_component>();
                            const gameplay_fabricator_shared_ptr gameplay_fabricator = scene_fabricator_component->gameplay_fabricator;
                            if (!m_level.expired())
                            {
                                const auto garage_database_component = root->get_component<ces_garage_database_component>();
                                auto selected_car_id = garage_database_component->get_previewed_car_id();
                                std::stringstream car_configuration_filename;
                                car_configuration_filename<<"car_0"<<selected_car_id;
                                gameplay_fabricator->reskin_car(main_car, car_configuration_filename.str(), 2);
                                garage_database_component->select_car_skin(1, selected_car_id, 2);
                            }
                        });
                    }
                }
                break;
                
                case ces_ui_interaction_component::e_ui::e_ui_car_skin_3_button:
                {
                    m_car_skin_3_button = entity;
                    if(!m_car_skin_3_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_car_skin_3_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            pop_current_dialog();
                            const auto main_car = std::static_pointer_cast<gb::game_object_3d>(m_camera_follow_car.lock());
                            const auto scene_fabricator_component = root->get_component<ces_scene_fabricator_component>();
                            const gameplay_fabricator_shared_ptr gameplay_fabricator = scene_fabricator_component->gameplay_fabricator;
                            if (!m_level.expired())
                            {
                                const auto garage_database_component = root->get_component<ces_garage_database_component>();
                                auto selected_car_id = garage_database_component->get_previewed_car_id();
                                std::stringstream car_configuration_filename;
                                car_configuration_filename<<"car_0"<<selected_car_id;
                                gameplay_fabricator->reskin_car(main_car, car_configuration_filename.str(), 3);
                                garage_database_component->select_car_skin(1, selected_car_id, 3);
                            }
                        });
                    }
                }
                break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_scores_label:
                {
                    m_scores_label = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_countdown_label:
                {
                    m_countdown_label = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_levels_list_dialog:
                {
                    m_levels_list_dialog = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_cars_list_dialog:
                {
                    m_cars_list_dialog = entity;
                    if (!m_cars_list_dialog.expired())
                    {
                        update_cars_list_dialog();
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_goto_racing_button:
                {
                    m_goto_racing_button = entity;
                    if(!m_goto_racing_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_goto_racing_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            pop_current_dialog();
                            
                            const auto levels_database_component = root->get_component<ces_levels_database_component>();
                            const auto levels = levels_database_component->get_all_levels();
                            
                            for (i32 i = 1; i <= levels.size(); ++i)
                            {
                                auto level_data_it = levels.find(i);
                                if (level_data_it != levels.end())
                                {
                                    if (level_data_it->second->get_is_openned() && !level_data_it->second->get_is_passed())
                                    {
                                        m_scene.lock()->get_component<ces_scene_state_automat_component>()->mode = ces_scene_state_automat_component::e_mode_in_game;
                                        m_scene.lock()->get_component<ces_scene_state_automat_component>()->state = ces_scene_state_automat_component::e_state_should_preload;
                                    }
                                }
                            }
                        });
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_pause_button:
                {
                    m_pause_button = entity;
                    if(!m_pause_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_pause_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            pop_current_dialog();
                            push_pause_menu_dialog(root);
                            const auto level_descriptor_component = m_level.lock()->get_component<ces_level_descriptor_component>();
                            level_descriptor_component->is_paused = true;
                        });
                    }
                }
                    break;
                                                                                                   
                case ces_ui_interaction_component::e_ui::e_ui_pause_menu_dialog:
                {
                    m_pause_menu_dialog = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_restart_dialog:
                {
                    m_restart_dialog = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_quit_dialog:
                {
                    m_quit_dialog = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_win_dialog:
                {
                    m_end_game_dialog = entity;
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_select_car_button:
                {
                    m_select_car_button = entity;
                    if(!m_select_car_button.lock()->as<gb::ui::image_button>()->is_pressed_callback_exist())
                    {
                        m_select_car_button.lock()->as<gb::ui::image_button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            if (!m_level.expired())
                            {
                                const auto garage_database_component = root->get_component<ces_garage_database_component>();
                                auto selected_car_id = garage_database_component->get_previewed_car_id();
                                garage_database_component->select_car(1, selected_car_id);
                            }
                            m_select_car_button.lock()->as<gb::ui::image_button>()->set_image_color(glm::u8vec4(64, 64, 255, 255));
                        });
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_ui::e_ui_unlock_car_button:
                {
                    m_unlock_car_button = entity;
                    if(!m_unlock_car_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_unlock_car_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                            
                        });
                    }
                }
                    break;
                    
                default:
                    break;
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
        
    }
    
    void ces_ui_interaction_system::pop_current_dialog()
    {
        if (!m_current_pushed_dialog.expired())
        {
            m_previous_pushed_dialog = m_current_pushed_dialog.lock();
            m_current_pushed_dialog.lock()->visible = false;
            m_current_pushed_dialog.reset();
        }
    }
    
    void ces_ui_interaction_system::push_levels_list_dialog(const gb::ces_entity_shared_ptr& root)
    {
        const auto levels_database_component = root->get_component<ces_levels_database_component>();
        const auto levels = levels_database_component->get_all_levels();
        m_levels_list_dialog.lock()->visible = true;
        m_current_pushed_dialog = m_levels_list_dialog.lock();
        
        const auto levels_list_table_view = std::static_pointer_cast<gb::ui::table_view>(std::static_pointer_cast<gb::ui::dialog>(m_levels_list_dialog.lock())->get_control(ces_ui_interaction_component::k_levels_list_dialog_levels_table));
        
        std::vector<gb::ui::table_view_cell_data_shared_ptr> data;
        for (i32 i = 1; i <= levels.size(); ++i)
        {
            auto level_data_it = levels.find(i);
            if (level_data_it != levels.end())
            {
                auto level_cell_data = std::make_shared<ui::levels_list_table_view_cell_data>();
                level_cell_data->id = level_data_it->second->get_id();
                bool is_locked = !level_data_it->second->get_is_openned();
                level_cell_data->is_locked = is_locked;
                data.push_back(level_cell_data);
            }
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
                cell->size = glm::vec2(256.f, 92.f);
                
                std::static_pointer_cast<ui::levels_list_table_view_cell>(cell)->set_start_level_button_callback_t([=](i32 index) {
                    auto data_source = std::static_pointer_cast<gb::ui::table_view>(table_view)->get_data_source();
                    auto data = data_source.at(index);
                    
                    m_scene.lock()->get_component<ces_scene_state_automat_component>()->mode = ces_scene_state_automat_component::e_mode_in_game;
                    m_scene.lock()->get_component<ces_scene_state_automat_component>()->state = ces_scene_state_automat_component::e_state_should_preload;
                    pop_current_dialog();
                });
            }
            
            const auto level_cell_data = std::static_pointer_cast<ui::levels_list_table_view_cell_data>(data);
            std::static_pointer_cast<ui::levels_list_table_view_cell>(cell)->set_index(level_cell_data->id);
            bool is_locked = level_cell_data->is_locked;
            std::static_pointer_cast<ui::levels_list_table_view_cell>(cell)->set_is_locked(is_locked);
    
            return cell;
        });
        levels_list_table_view->set_on_get_table_cell_height_callback([](i32 index) {
            return 96.f;
        });
        levels_list_table_view->reload_data();
    }
    
    void ces_ui_interaction_system::push_pause_menu_dialog(const gb::ces_entity_shared_ptr& root)
    {
        m_pause_menu_dialog.lock()->visible = true;
        m_current_pushed_dialog = m_pause_menu_dialog.lock();
        
        const auto continue_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(m_pause_menu_dialog.lock())->get_control(ces_ui_interaction_component::k_pause_menu_dialog_continue_button));
        
        if(!continue_button->is_pressed_callback_exist())
        {
            continue_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                pop_current_dialog();
                const auto level_descriptor_component = m_level.lock()->get_component<ces_level_descriptor_component>();
                level_descriptor_component->is_paused = false;
            });
        }
        
        const auto restart_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(m_pause_menu_dialog.lock())->get_control(ces_ui_interaction_component::k_pause_menu_dialog_restart_button));
        
        if(!restart_button->is_pressed_callback_exist())
        {
            restart_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                pop_current_dialog();
                push_restart_dialog(root);
            });
        }
        
        const auto quit_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(m_pause_menu_dialog.lock())->get_control(ces_ui_interaction_component::k_pause_menu_dialog_exit_button));
        
        if(!quit_button->is_pressed_callback_exist())
        {
            quit_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                pop_current_dialog();
                push_quit_dialog(root);
            });
        }
    }
    
    void ces_ui_interaction_system::push_restart_dialog(const gb::ces_entity_shared_ptr& root)
    {
        m_restart_dialog.lock()->visible = true;
        m_current_pushed_dialog = m_restart_dialog.lock();
        
        const auto yes_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(m_restart_dialog.lock())->get_control(ces_ui_interaction_component::k_restart_dialog_yes_button));
        
        if(!yes_button->is_pressed_callback_exist())
        {
            yes_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                pop_current_dialog();
                const auto level_descriptor_component = m_level.lock()->get_component<ces_level_descriptor_component>();
                level_descriptor_component->is_paused = false;
                m_scene.lock()->get_component<ces_scene_state_automat_component>()->mode = ces_scene_state_automat_component::e_mode_in_game;
                m_scene.lock()->get_component<ces_scene_state_automat_component>()->state = ces_scene_state_automat_component::e_state_should_preload;
            });
        }
        
        const auto no_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(m_restart_dialog.lock())->get_control(ces_ui_interaction_component::k_restart_dialog_no_button));
        
        if(!no_button->is_pressed_callback_exist())
        {
            no_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                pop_current_dialog();
                push_pause_menu_dialog(root);
            });
        }
    }
    
    void ces_ui_interaction_system::push_quit_dialog(const gb::ces_entity_shared_ptr& root)
    {
        m_quit_dialog.lock()->visible = true;
        m_current_pushed_dialog = m_quit_dialog.lock();
        
        const auto yes_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(m_quit_dialog.lock())->get_control(ces_ui_interaction_component::k_quit_dialog_yes_button));
        
        if(!yes_button->is_pressed_callback_exist())
        {
            yes_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                pop_current_dialog();
                const auto level_descriptor_component = m_level.lock()->get_component<ces_level_descriptor_component>();
                level_descriptor_component->is_paused = false;
                m_scene.lock()->get_component<ces_scene_state_automat_component>()->mode = ces_scene_state_automat_component::e_mode_main_menu;
                m_scene.lock()->get_component<ces_scene_state_automat_component>()->state = ces_scene_state_automat_component::e_state_should_preload;
            });
        }
        
        const auto no_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(m_quit_dialog.lock())->get_control(ces_ui_interaction_component::k_quit_dialog_no_button));
        
        if(!no_button->is_pressed_callback_exist())
        {
            no_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                pop_current_dialog();
                push_pause_menu_dialog(root);
            });
        }
    }
    
    void ces_ui_interaction_system::push_win_dialog(const gb::ces_entity_shared_ptr& root)
    {
        m_end_game_dialog.lock()->visible = true;
        m_current_pushed_dialog = m_end_game_dialog.lock();
        
        const auto continue_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(m_end_game_dialog.lock())->get_control(ces_ui_interaction_component::k_end_game_dialog_continue_button));
        
        if(!continue_button->is_pressed_callback_exist())
        {
            continue_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                pop_current_dialog();
                m_scene.lock()->get_component<ces_scene_state_automat_component>()->mode = ces_scene_state_automat_component::e_mode_main_menu;
                m_scene.lock()->get_component<ces_scene_state_automat_component>()->state = ces_scene_state_automat_component::e_state_should_preload;
            });
        }
        
        const auto restart_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(m_end_game_dialog.lock())->get_control(ces_ui_interaction_component::k_end_game_dialog_restart_button));
        
        if(!restart_button->is_pressed_callback_exist())
        {
            restart_button->set_on_pressed_callback([=](const gb::ces_entity_shared_ptr&) {
                pop_current_dialog();
                m_scene.lock()->get_component<ces_scene_state_automat_component>()->mode = ces_scene_state_automat_component::e_mode_in_game;
                m_scene.lock()->get_component<ces_scene_state_automat_component>()->state = ces_scene_state_automat_component::e_state_should_preload;
            });
        }
        
        const auto car_descriptor_component = m_main_car.lock()->get_component<ces_car_descriptor_component>();
        const auto car_drift_state_component = m_main_car.lock()->get_component<ces_car_drift_state_component>();
        i32 place = car_descriptor_component->place;
        f32 drift_time = car_drift_state_component->total_drifting_time;
        
        i32 stars_count = 0;
        
        const auto place_label = std::static_pointer_cast<gb::ui::textfield>(std::static_pointer_cast<gb::ui::dialog>(m_end_game_dialog.lock())->get_control(ces_ui_interaction_component::k_end_game_dialog_place_label));
        
        const auto star1_image = std::static_pointer_cast<gb::sprite>(std::static_pointer_cast<gb::ui::dialog>(m_end_game_dialog.lock())->get_control(ces_ui_interaction_component::k_end_game_dialog_star1_image));
        
         const auto star2_image = std::static_pointer_cast<gb::sprite>(std::static_pointer_cast<gb::ui::dialog>(m_end_game_dialog.lock())->get_control(ces_ui_interaction_component::k_end_game_dialog_star2_image));
        
         const auto star3_image = std::static_pointer_cast<gb::sprite>(std::static_pointer_cast<gb::ui::dialog>(m_end_game_dialog.lock())->get_control(ces_ui_interaction_component::k_end_game_dialog_star3_image));
        
        const auto star1_achievment_label = std::static_pointer_cast<gb::ui::textfield>(std::static_pointer_cast<gb::ui::dialog>(m_end_game_dialog.lock())->get_control(ces_ui_interaction_component::k_win_dialog_star1_achievement_label));
        
        const auto star2_achievment_label = std::static_pointer_cast<gb::ui::textfield>(std::static_pointer_cast<gb::ui::dialog>(m_end_game_dialog.lock())->get_control(ces_ui_interaction_component::k_win_dialog_star2_achievement_label));
        
        const auto star3_achievment_label = std::static_pointer_cast<gb::ui::textfield>(std::static_pointer_cast<gb::ui::dialog>(m_end_game_dialog.lock())->get_control(ces_ui_interaction_component::k_win_dialog_star3_achievement_label));
    
        star1_image->color = glm::u8vec4(32, 32, 32, 255);
        star2_image->color = glm::u8vec4(32, 32, 32, 255);
        star3_image->color = glm::u8vec4(32, 32, 32, 255);
        
        star1_achievment_label->set_font_color(glm::u8vec4(255, 64, 64, 255));
        star2_achievment_label->set_font_color(glm::u8vec4(255, 64, 64, 255));
        star3_achievment_label->set_font_color(glm::u8vec4(255, 64, 64, 255));
        
        if (place == 1)
        {
            place_label->set_text("FINISHED FIRST");
            stars_count++;
            star2_achievment_label->set_font_color(glm::u8vec4(64, 255, 64, 255));
            star2_image->color = glm::u8vec4(192, 0, 192, 255);
        }
        else if (place == 2)
        {
            place_label->set_text("FINISHED SECOND");
        }
        else if (place == 3)
        {
            place_label->set_text("FINISHED THIRD");
        }
        else if (place == 4)
        {
            place_label->set_text("FINISHED LAST");
        }
        
        f32 current_damage = car_descriptor_component->current_damage;
        if (current_damage == 0.f)
        {
            star1_achievment_label->set_font_color(glm::u8vec4(64, 255, 64, 255));
            star1_image->color = glm::u8vec4(192, 0, 192, 255);
        }
        
        const auto drift_time_label = std::static_pointer_cast<gb::ui::textfield>(std::static_pointer_cast<gb::ui::dialog>(m_end_game_dialog.lock())->get_control(ces_ui_interaction_component::k_end_game_dialog_drift_time_label));
        
        i32 seconds = drift_time / 1000;
        f32 f_milliseconds = drift_time / 1000 - seconds;
        i32 milliseconds = f_milliseconds * 10;
        
        std::stringstream drift_value_string_stream;
        drift_value_string_stream<<"DRIFT TIME: "<<(seconds < 10 ? "0" : "")<<seconds<<":"<<milliseconds<<"0"<<" sec";
        drift_time_label->set_text(drift_value_string_stream.str());
        
        if (seconds >= 30)
        {
            star3_achievment_label->set_font_color(glm::u8vec4(64, 255, 64, 255));
            star3_image->color = glm::u8vec4(192, 0, 192, 255);
        }
    }
    
    void ces_ui_interaction_system::update_cars_list_dialog()
    {
        const auto car_list_table_view = std::static_pointer_cast<gb::ui::table_view>(std::static_pointer_cast<gb::ui::dialog>(m_cars_list_dialog.lock())->get_control(ces_ui_interaction_component::k_cars_list_dialog_table));
        bool is_data_globally_changed = false;
        std::vector<gb::ui::table_view_cell_data_shared_ptr> data = car_list_table_view->get_data_source();
        for (auto car_it : m_all_cars) {
            if (!car_it.second.expired())
            {
                const auto data_it = std::find_if(data.begin(), data.end(), [=](const gb::ui::table_view_cell_data_shared_ptr& car_data_it) {
                    const auto car_data = std::static_pointer_cast<ui::cars_list_table_view_cell_data>(car_data_it);
                    std::string car_data_id = car_data->id;
                    std::string car_id = car_it.second.lock()->tag;
                    if (car_id == car_data_id)
                    {
                        return true;
                    }
                    return false;
                });
                
                if (data_it == data.end())
                {
                    auto car_data = std::make_shared<ui::cars_list_table_view_cell_data>();
                    std::string car_id = car_it.second.lock()->tag;
                    car_data->id = car_id;
                    data.push_back(car_data);
                    is_data_globally_changed = true;
                }
            }
        }
        if (is_data_globally_changed)
        {
            car_list_table_view->set_data_source(data);
            car_list_table_view->set_on_get_cell_callback([=](i32 index, const gb::ui::table_view_cell_data_shared_ptr& data, const gb::ces_entity_shared_ptr& table_view) {
                gb::ui::table_view_cell_shared_ptr cell = nullptr;
                cell = std::static_pointer_cast<gb::ui::table_view>(table_view)->reuse_cell("car_cell", index);
                if(!cell)
                {
                    cell = gb::ces_entity::construct<ui::cars_list_table_view_cell>(std::static_pointer_cast<gb::ui::table_view>(table_view)->get_fabricator(),
                                                                                    index, "car_cell");
                    cell->create();
                    cell->size = glm::vec2(130.f, 28.f);
                }
                
                const auto car_data = std::static_pointer_cast<ui::cars_list_table_view_cell_data>(data);
                const auto car_cell = std::static_pointer_cast<ui::cars_list_table_view_cell>(cell);
                i32 car_place = car_data->place;
                std::string racer_name = car_data->name;
                car_cell->place = car_place;
                car_cell->name = racer_name;
                return cell;
            });
            car_list_table_view->set_on_get_table_cell_height_callback([](i32 index) {
                return 28.f;
            });
            car_list_table_view->set_on_update_cell_callback([=](i32 index, const gb::ui::table_view_cell_data_shared_ptr& data, const gb::ces_entity_shared_ptr& table_view, gb::ui::table_view_cell_shared_ptr cell) {

                const auto car_data = std::static_pointer_cast<ui::cars_list_table_view_cell_data>(data);
                const auto car_cell = std::static_pointer_cast<ui::cars_list_table_view_cell>(cell);
                i32 car_place = car_data->place;
                std::string racer_name = car_data->name;
                f32 drift_time = car_data->drift_time;
                car_cell->place = car_place;
                car_cell->name = racer_name;
                car_cell->drift_time = drift_time;
            });
            car_list_table_view->reload_data();
        }
        
        for (auto car_it : m_all_cars)
        {
            if (!car_it.second.expired())
            {
                const auto car_descriptor_component = car_it.second.lock()->get_component<ces_car_descriptor_component>();
                const auto car_drift_state_component = car_it.second.lock()->get_component<ces_car_drift_state_component>();
                i32 place = car_descriptor_component->place;
                const auto car_data = std::static_pointer_cast<ui::cars_list_table_view_cell_data>(data.at(place - 1));
                
                std::string car_id = car_it.second.lock()->tag;
                std::string racer_name = car_descriptor_component->racer_name;
                f32 total_drift_time = car_drift_state_component->total_drifting_time;
                car_data->id = car_id;
                car_data->place = place;
                car_data->name = racer_name;
                car_data->drift_time = total_drift_time;
            }
        }
        car_list_table_view->update_data();
    }
    
    void ces_ui_interaction_system::on_dragging(const gb::ces_entity_shared_ptr& entity, const glm::vec2& delta)
    {

    }
    
    void ces_ui_interaction_system::on_drag_ended(const gb::ces_entity_shared_ptr& entity, const glm::vec2& point)
    {
      
    }
}
