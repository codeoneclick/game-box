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
#include "dialog.h"
#include "button.h"
#include "action_console.h"
#include "game_object_2d.h"
#include "ai_actions_processor.h"
#include "ai_attack_action.h"
#include "ai_chase_action.h"
#include "information_bubble_controller.h"
#include "ces_character_pathfinder_component.h"
#include "ces_quest_giver_component.h"
#include "ces_quest_receiver_component.h"
#include "ces_ui_quest_dialog_component.h"

namespace game
{
    ces_ui_interaction_system::ces_ui_interaction_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_controllers_component::class_guid());
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_path_grid_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);

        ces_base_system::add_required_component_guid(m_ui_components_mask, ces_ui_interaction_component::class_guid());
        ces_base_system::add_required_components_mask(m_ui_components_mask);
        
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_controllers_component::class_guid());
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
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_character_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            std::string character_key = entity->tag;
            auto character_controllers_component = entity->get_component<ces_character_controllers_component>();
            if(character_controllers_component->mode == ces_character_controllers_component::e_mode::e_mode_manual)
            {
                m_main_character = entity;
            }
            m_all_characters[character_key] = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_ui_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            auto ui_interaction_component = entity->get_component<ces_ui_interaction_component>();
            switch (ui_interaction_component->get_type())
            {
                case ces_ui_interaction_component::e_type_attack_button:
                {
                    m_attack_button = entity;
                    ces_ui_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_pressed);
                    ces_ui_interaction_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_released);
                }
                    break;
                    
                case ces_ui_interaction_component::e_type_quest_dialog:
                {
                    m_quest_dialog = entity;
                    
                    const auto& character_selector_component = m_main_character.lock()->get_component<ces_character_selector_component>();
                    auto accept_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(entity)->get_control(ces_ui_interaction_component::k_quest_dialog_accept_button));
                    auto decline_button = std::static_pointer_cast<gb::ui::button>(std::static_pointer_cast<gb::ui::dialog>(entity)->get_control(ces_ui_interaction_component::k_quest_dialog_decline_button));
                    if(!accept_button->is_pressed_callback_exist())
                    {
                        accept_button->set_on_pressed_callback([this, character_selector_component](const gb::ces_entity_shared_ptr&){
                            auto ui_quest_dialog_component = m_quest_dialog.lock()->get_component<ces_ui_quest_dialog_component>();
                            if(ui_quest_dialog_component->is_selected_quest_id_exist())
                            {
                                auto opponent_character = character_selector_component->get_selections().at(0).lock();
                                const auto& quest_giver_component = opponent_character->get_component<ces_quest_giver_component>();
                                const auto& quest_receiver_component = m_main_character.lock()->get_component<ces_quest_receiver_component>();
                                quest_receiver_component->add_to_quest_log(ui_quest_dialog_component->get_selected_quest_id(), quest_giver_component->get_quest(ui_quest_dialog_component->get_selected_quest_id()));
                            }
                            character_selector_component->remove_all_selections();
                        });
                    }
                    if(!decline_button->is_pressed_callback_exist())
                    {
                        decline_button->set_on_pressed_callback([character_selector_component](const gb::ces_entity_shared_ptr&){
                            character_selector_component->remove_all_selections();
                        });
                    }
                    
                    if(!character_selector_component->is_selections_exist())
                    {
                        entity->visible = false;
                        auto ui_quest_dialog_component = m_quest_dialog.lock()->get_component<ces_ui_quest_dialog_component>();
                        ui_quest_dialog_component->set_selected_quest_id(-1);
                    }
                    else
                    {
                        auto selected_opponent = character_selector_component->get_selections().at(0);
                        auto character_state_automat_component = selected_opponent.lock()->get_component<ces_character_state_automat_component>();
                        if(character_state_automat_component->get_mode() != ces_character_state_automat_component::e_mode_npc)
                        {
                            entity->visible = false;
                            auto ui_quest_dialog_component = m_quest_dialog.lock()->get_component<ces_ui_quest_dialog_component>();
                            ui_quest_dialog_component->set_selected_quest_id(-1);
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
                        const auto& opponent_character_state_automat_component = opponent_character->get_component<ces_character_state_automat_component>();
                        if(opponent_character_state_automat_component->get_mode() != ces_character_state_automat_component::e_mode_npc)
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
                                        auto ui_quest_dialog_component = m_quest_dialog.lock()->get_component<ces_ui_quest_dialog_component>();
                                        ui_quest_dialog_component->set_selected_quest_id(it);
                                        break;
                                    }
                                }
                                m_quest_dialog.lock()->visible = is_available_quest_exist;
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
}
