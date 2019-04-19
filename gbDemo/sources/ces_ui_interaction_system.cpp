//
//  ces_ui_interaction_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/27/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_ui_interaction_system.h"
#include "ces_character_controllers_component.h"
#include "ces_character_state_automat_component.h"
#include "ces_character_statistic_component.h"
#include "ces_character_selector_component.h"
#include "ces_character_parts_component.h"
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

namespace game
{
    ces_ui_interaction_system::ces_ui_interaction_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_controllers_component::class_guid());
        //ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_path_grid_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);

        ces_base_system::add_required_component_guid(m_ui_components_mask, ces_ui_interaction_component::class_guid());
        ces_base_system::add_required_components_mask(m_ui_components_mask);
        
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
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
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
                const auto car_parts_component = main_character->get_component<ces_character_parts_component>();
                const auto car_drift_state_component = main_character->get_component<ces_car_drift_state_component>();
                const auto car_gear_component = main_character->get_component<ces_car_gear_component>();
               
                const auto speed_value_label = std::static_pointer_cast<gb::label_3d>(car_parts_component->get_part(ces_character_parts_component::parts::k_ui_speed_value_label));
                //character_navigation_component->update(dt);
                //const auto velocity = character_navigation_component->get_velocity();
                //const auto rotation = character_navigation_component->get_rotation();
                
                //car_input_component->updated = true;
                //car_input_component->throttle = car_model_component->get_max_force();
                //car_input_component->steer_angle = rotation;
                
                glm::vec3 current_position = main_character->position;
                glm::vec3 current_rotation = main_character->rotation;
                
                glm::vec2 velocity_wc = car_descriptor_component->velocity_wc;
                f32 velocity_wc_length = glm::length(velocity_wc);
                f32 current_velocity_length_squared = velocity_wc_length * velocity_wc_length;
                f32 max_speed_squared = car_model_component->get_max_speed() * car_model_component->get_max_speed();
                f32 current_speed_factor = glm::clamp(current_velocity_length_squared / max_speed_squared, 0.f, 1.f);
                
                current_position.x += velocity_wc.x * current_speed_factor * .33f;
                current_position.z += velocity_wc.y * current_speed_factor * .33f;
                const auto camera_3d = ces_base_system::get_current_camera_3d();
                auto current_look_at = camera_3d->get_look_at();
                current_look_at = glm::mix(current_look_at, current_position, glm::clamp(.1f, 1.f, 1.f - current_speed_factor));
                camera_3d->set_look_at(current_look_at);
                
                auto current_camera_rotation = camera_3d->get_rotation();
                current_camera_rotation = glm::mix(current_camera_rotation, glm::degrees(current_rotation.y) - 90.f, .05f);
                camera_3d->set_rotation(current_camera_rotation);
                
                camera_3d->set_distance_to_look_at(glm::vec3(glm::mix(12.f, 24.f, current_speed_factor),
                                                             glm::mix(24.f, 32.f, current_speed_factor),
                                                             glm::mix(12.f, 24.f, current_speed_factor)));
                
                
                i32 rpm = car_gear_component->get_rpm(current_speed_factor, car_gear_component->get_previous_load());
                
                std::stringstream string_stream;
                string_stream<<rpm<<" rpm";
                speed_value_label->text = string_stream.str();
                
                
            }
            m_all_characters[character_key] = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_ui_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            auto ui_interaction_component = entity->get_component<ces_ui_interaction_component>();
            switch (ui_interaction_component->get_type())
            {
                case ces_ui_interaction_component::e_type_move_joystick:
                {
                    m_move_joystick = entity;
                    const auto move_joystick = std::static_pointer_cast<gb::ui::joystick>(entity);
                    if (!move_joystick->is_dragging_callback_exist())
                    {
                        move_joystick->set_on_dragging_callback(std::bind(&ces_ui_interaction_system::on_dragging, this,
                                                                          std::placeholders::_1, std::placeholders::_2));
                    }
                    if (!move_joystick->is_drag_ended_callback_exist())
                    {
                        move_joystick->set_on_drag_ended_callback(std::bind(&ces_ui_interaction_system::on_drag_ended, this,
                                                                            std::placeholders::_1, std::placeholders::_2));
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_type_attack_button:
                {
                    m_attack_button = entity;
                    ces_ui_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_pressed);
                    ces_ui_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_released);
                }
                    break;
                    
                case ces_ui_interaction_component::e_type_questlog_button:
                {
                    m_questlog_button = entity;
                    if(!m_questlog_button.lock()->as<gb::ui::button>()->is_pressed_callback_exist())
                    {
                        m_questlog_button.lock()->as<gb::ui::button>()->set_on_pressed_callback([this](const gb::ces_entity_shared_ptr&) {
                            ces_ui_interaction_system::show_questlog_dialog();
                        });
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_type_questlog_dialog:
                {
                    m_questlog_dialog = entity;
                    auto close_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(entity)->get_control(ces_ui_interaction_component::k_questlog_dialog_close_button));
                    if(!close_button->is_pressed_callback_exist())
                    {
                        close_button->set_on_pressed_callback([this](const gb::ces_entity_shared_ptr&) {
                            m_questlog_dialog.lock()->visible = false;
                        });
                    }
                }
                    break;
                    
                case ces_ui_interaction_component::e_type_quest_dialog:
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
        if(entity == m_attack_button.lock() && input_state == gb::e_input_state_released)
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
                        const auto& character_state_automat_component = current_character->get_component<ces_character_state_automat_component>();
                        auto actions_processor = character_state_automat_component->get_actions_processor();
                        actions_processor->interrupt_all_actions();
                        auto character_statistic_component = opponent_character->get_component<ces_character_statistic_component>();
                        if(character_statistic_component->mode != ces_character_statistic_component::e_mode_npc)
                        {
                            auto attack_action = std::make_shared<ai_attack_action>(current_character);
                            attack_action->set_parameters(std::static_pointer_cast<gb::game_object_2d>(opponent_character));
                            attack_action->set_start_callback([](const ai_action_shared_ptr& action) {
                                auto character = action->get_owner();
                                const auto& character_state_automat_component = character->get_component<ces_character_state_automat_component>();
                                character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_attack);
                            });
                            attack_action->set_end_callback([](const ai_action_shared_ptr& action) {
                                auto character = action->get_owner();
                                const auto& character_state_automat_component = character->get_component<ces_character_state_automat_component>();
                                character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_idle);
                            });
                            actions_processor->add_action(attack_action);
                        }
                        else
                        {
                            character_state_automat_component->set_state(game::ces_character_state_automat_component::e_state_idle);
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
                    std::static_pointer_cast<gb::ui::action_console>(m_action_console.lock())->write("I need to be closer");

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
        }
    }
    
    void ces_ui_interaction_system::show_quests_dialog()
    {
        m_quests_dialog.lock()->visible = true;
        const auto& character_selector_component = m_main_character.lock()->get_component<ces_character_selector_component>();
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
                        ces_ui_interaction_system::show_quests_dialog();
                    });
                }
                std::static_pointer_cast<ces_ui_quest_dialog_component::quest_table_view_cell>(cell)->set_quest_in_progress(quest_receiver_component->is_quest_exist(std::static_pointer_cast<ces_ui_quest_dialog_component::quest_table_view_cell_data>(data)->get_quest_id()));
                
                return cell;
            });
            quests_table_view->set_on_get_table_cell_height_callback([](i32 index) {
                return 96.f;
            });
            quests_table_view->reload_data();
        }
    }
    
    void ces_ui_interaction_system::show_questlog_dialog()
    {
        m_questlog_dialog.lock()->visible = true;
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
        }
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
