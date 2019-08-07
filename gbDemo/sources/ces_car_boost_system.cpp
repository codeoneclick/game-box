//
//  ces_car_boost_system.cpp
//  gbDemo
//
//  Created by serhii.s on 8/6/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_car_boost_system.h"
#include "ces_level_descriptor_component.h"
#include "ces_car_statistic_component.h"
#include "ces_bound_touch_3d_component.h"
#include "ces_level_descriptor_component.h"
#include "game_object_2d.h"
#include "game_object_3d.h"
#include "camera_2d.h"
#include "camera_3d.h"
#include "glm_extensions.h"
#include "ces_level_route_component.h"
#include "ces_car_descriptor_component.h"
#include "ces_car_impact_component.h"
#include "ces_box2d_world_component.h"
#include "ui_controls_helper.h"
#include "ui_animation_helper.h"
#include "ces_ui_move_animation_action_component.h"
#include "progress_bar.h"

namespace game
{
    ces_car_boost_system::ces_car_boost_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_descriptor_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_car_components_mask, ces_car_statistic_component::class_guid());
        ces_base_system::add_required_components_mask(m_car_components_mask);
    }
    
    void ces_car_boost_system::on_feed_start(f32 dt)
    {
    }
    
    void ces_car_boost_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_car_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            auto character_statistic_component = entity->get_component<ces_car_statistic_component>();
            if(character_statistic_component->mode == ces_car_statistic_component::e_mode::e_mode_player)
            {
                m_main_car = entity;
            }
        });
        
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
        });
        
        if (!m_main_car.expired())
        {
            const auto car = std::static_pointer_cast<gb::game_object_3d>(m_main_car.lock());
            const auto level_descriptor_component = m_level.lock()->get_component<ces_level_descriptor_component>();
            const auto car_descriptor_component = car->get_component<ces_car_descriptor_component>();
            const auto car_impact_component = car->get_component<ces_car_impact_component>();
            
            f32 complexity = level_descriptor_component->complexity;
            
            if (car_impact_component->get_is_expect_to_speed_up_impact())
            {
                car_impact_component->reset_is_expect_to_speed_up_impact();
                if (!car_impact_component->is_speed_up_impact_exist())
                {
                    car_impact_component->add_speed_up_impact();
                    car_impact_component->remove_slow_motion_impact();
                }
            }
            
            if (car_impact_component->is_slow_motion_impact_exist())
            {
                auto current_box2d_update_interval = root->get_component<gb::ces_box2d_world_component>()->get_update_interval();
                current_box2d_update_interval = glm::mix(current_box2d_update_interval, glm::mix(.25f, .5f, complexity), .33f);
                root->get_component<gb::ces_box2d_world_component>()->set_update_interval(current_box2d_update_interval);
                car_impact_component->update_current_slow_motion_effect_duration_in_seconds(-dt);
                
                if (!car_impact_component->get_is_slow_motion_boost_ui_shown())
                {
                    const auto slow_motion_boost_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_slow_motion_boost_label);
                    slow_motion_boost_label->visible = true;
                    auto ui_move_animation_action_component = slow_motion_boost_label->get_component<ces_ui_move_animation_action_component>();
                    if (!ui_move_animation_action_component)
                    {
                        ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                        slow_motion_boost_label->add_component(ui_move_animation_action_component);
                    }
                    ui_move_animation_action_component->set_duration_in_second(.33f);
                    ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
                    ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_left);
                    ui_animation_helper::move_with_animation_action(slow_motion_boost_label, [=]() {
                        car_impact_component->set_is_slow_motion_boost_ui_shown(true);
                        slow_motion_boost_label->focus(true, .33f);
                    });
                    
                    const auto slow_motion_boost_progress_bar = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_slow_motion_progress_bar);
                    slow_motion_boost_progress_bar->visible = true;
                    ui_move_animation_action_component = slow_motion_boost_progress_bar->get_component<ces_ui_move_animation_action_component>();
                    if (!ui_move_animation_action_component)
                    {
                        ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                        slow_motion_boost_progress_bar->add_component(ui_move_animation_action_component);
                    }
                    ui_move_animation_action_component->set_duration_in_second(.33f);
                    ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
                    ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_left);
                    ui_animation_helper::move_with_animation_action(slow_motion_boost_progress_bar, [=]() {
                        car_impact_component->set_is_slow_motion_boost_ui_shown(true);
                        slow_motion_boost_progress_bar->focus(true, .33f);
                    });
                }
                const auto slow_motion_boost_progress_bar = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_slow_motion_progress_bar);
                std::static_pointer_cast<gb::ui::progress_bar>(slow_motion_boost_progress_bar)->set_progress(car_impact_component->get_slow_motion_impact_progress());
            }
            else
            {
                auto current_box2d_update_interval = root->get_component<gb::ces_box2d_world_component>()->get_update_interval();
                current_box2d_update_interval = glm::mix(current_box2d_update_interval, 1.f, .33f);
                root->get_component<gb::ces_box2d_world_component>()->set_update_interval(current_box2d_update_interval);
                
                if (car_impact_component->get_is_slow_motion_boost_ui_shown())
                {
                    const auto slow_motion_boost_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_slow_motion_boost_label);
                    const auto slow_motion_boost_progress_bar = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_slow_motion_progress_bar);
                    slow_motion_boost_label->focus(false);
                    slow_motion_boost_progress_bar->focus(false);
                    
                    auto ui_move_animation_action_component = slow_motion_boost_label->get_component<ces_ui_move_animation_action_component>();
                    if (!ui_move_animation_action_component)
                    {
                        ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                        slow_motion_boost_label->add_component(ui_move_animation_action_component);
                    }
                    ui_move_animation_action_component->set_duration_in_second(.33f);
                    ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
                    ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_right);
                    ui_animation_helper::move_with_animation_action(slow_motion_boost_label, [=]() {
                        car_impact_component->set_is_slow_motion_boost_ui_shown(false);
                    });
                    
                    ui_move_animation_action_component = slow_motion_boost_progress_bar->get_component<ces_ui_move_animation_action_component>();
                    if (!ui_move_animation_action_component)
                    {
                        ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                        slow_motion_boost_progress_bar->add_component(ui_move_animation_action_component);
                    }
                    ui_move_animation_action_component->set_duration_in_second(.33f);
                    ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
                    ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_right);
                    ui_animation_helper::move_with_animation_action(slow_motion_boost_progress_bar, [=]() {
                        car_impact_component->set_is_slow_motion_boost_ui_shown(false);
                    });
                    
                }
            }
            
            if (car_impact_component->is_speed_up_impact_exist())
            {
                car_impact_component->update_current_speed_up_effect_duration_in_seconds(-dt);
            }
            
            i32 nearest_slow_motion_trigger_index = car_descriptor_component->nearest_slow_motion_trigger_index;
            glm::vec3 car_position = car->position;
            const auto level_route_component = m_level.lock()->get_component<ces_level_route_component>();
            std::vector<glm::vec2> slow_motion_triggers = level_route_component->slow_motion_triggers;
            f32 distance = glm::distance(glm::vec2(car_position.x, car_position.z), slow_motion_triggers.at(nearest_slow_motion_trigger_index));
            
            f32 distance_to_activate_trigger = car_descriptor_component->distance_to_activate_trigger;
            if (distance < distance_to_activate_trigger)
            {
                if (!car_impact_component->get_is_slow_motion_boost_trigger_ui_shown() && !car_impact_component->is_slow_motion_impact_exist())
                {
                    const auto slow_motion_boost_trigger_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_slow_motion_boost_trigger_label);
                    slow_motion_boost_trigger_label->visible = true;
                    auto ui_move_animation_action_component = slow_motion_boost_trigger_label->get_component<ces_ui_move_animation_action_component>();
                    if (!ui_move_animation_action_component)
                    {
                        ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                        slow_motion_boost_trigger_label->add_component(ui_move_animation_action_component);
                    }
                    ui_move_animation_action_component->set_duration_in_second(.33f);
                    ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_show);
                    ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_top);
                    ui_animation_helper::move_with_animation_action(slow_motion_boost_trigger_label, [=]() {
                        car_impact_component->set_is_slow_motion_boost_trigger_ui_shown(true);
                        slow_motion_boost_trigger_label->focus(true, .33f);
                    });
                }
                
                if (car_impact_component->get_is_expect_to_slow_motion_impact())
                {
                    car_impact_component->reset_is_expect_to_slow_motion_impact();
                    if (!car_impact_component->is_slow_motion_impact_exist())
                    {
                        car_impact_component->add_slow_motion_impact();
                        car_impact_component->remove_speed_up_impact();
                        
                        const auto slow_motion_boost_trigger_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_slow_motion_boost_trigger_label);
                        slow_motion_boost_trigger_label->focus(false);
                        auto ui_move_animation_action_component = slow_motion_boost_trigger_label->get_component<ces_ui_move_animation_action_component>();
                        if (!ui_move_animation_action_component)
                        {
                            ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                            slow_motion_boost_trigger_label->add_component(ui_move_animation_action_component);
                        }
                        ui_move_animation_action_component->set_duration_in_second(.33f);
                        ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
                        ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
                        ui_animation_helper::move_with_animation_action(slow_motion_boost_trigger_label, [=]() {
                            car_impact_component->set_is_slow_motion_boost_trigger_ui_shown(false);
                        });
                    }
                }
            }
            else
            {
                if (car_impact_component->get_is_slow_motion_boost_trigger_ui_shown())
                {
                    const auto slow_motion_boost_trigger_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_slow_motion_boost_trigger_label);
                    slow_motion_boost_trigger_label->focus(false);

                    auto ui_move_animation_action_component = slow_motion_boost_trigger_label->get_component<ces_ui_move_animation_action_component>();
                    if (!ui_move_animation_action_component)
                    {
                        ui_move_animation_action_component = std::make_shared<ces_ui_move_animation_action_component>();
                        slow_motion_boost_trigger_label->add_component(ui_move_animation_action_component);
                    }
                    ui_move_animation_action_component->set_duration_in_second(.33f);
                    ui_move_animation_action_component->set_move_mode(ces_ui_move_animation_action_component::e_move_mode::e_hide);
                    ui_move_animation_action_component->set_move_direction(ces_ui_move_animation_action_component::e_move_direction::e_down);
                    ui_animation_helper::move_with_animation_action(slow_motion_boost_trigger_label, [=]() {
                        car_impact_component->set_is_slow_motion_boost_trigger_ui_shown(false);
                    });
                }
                
                if (car_impact_component->get_is_expect_to_slow_motion_impact())
                {
                    car_impact_component->reset_is_expect_to_slow_motion_impact();
                }
            }
        }
    }
    
    void ces_car_boost_system::on_feed_end(f32 dt)
    {
        
    }
}
