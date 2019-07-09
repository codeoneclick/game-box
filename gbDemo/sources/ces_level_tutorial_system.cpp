//
//  ces_level_tutorial_system.cpp
//  gbDemo
//
//  Created by serhii.s on 5/24/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_level_tutorial_system.h"
#include "ces_transformation_2d_component.h"
#include "ces_car_parts_component.h"
#include "ces_scene_state_automat_component.h"
#include "std_extensions.h"
#include "glm_extensions.h"
#include "game_object_3d.h"
#include "textfield.h"
#include "ces_geometry_component.h"
#include "ces_level_route_component.h"
#include "ces_car_ai_input_component.h"
#include "ces_car_model_component.h"
#include "ces_car_descriptor_component.h"
#include "ces_car_input_component.h"
#include "ces_car_drift_state_component.h"
#include "ces_level_tutorial_component.h"
#include "ces_box2d_world_component.h"
#include "ces_ui_interaction_component.h"
#include "ui_controls_helper.h"
#include "ces_level_descriptor_component.h"

namespace game
{
    ces_level_tutorial_system::ces_level_tutorial_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_route_component::class_guid());
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_tutorial_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_cars_components_mask, ces_car_descriptor_component::class_guid());
        ces_base_system::add_required_components_mask(m_cars_components_mask);
        
        ces_base_system::add_required_component_guid(m_ui_components_mask, ces_ui_interaction_component::class_guid());
        ces_base_system::add_required_components_mask(m_ui_components_mask);
    }
    
    void ces_level_tutorial_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_level_tutorial_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_cars_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            std::string tag = entity->tag;
            m_cars[tag] = entity;
            if (entity->get_component<ces_car_input_component>())
            {
                m_main_car = entity;
            }
        });
        
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
        });
        
        ces_base_system::enumerate_entities_with_components(m_ui_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
            auto ui_interaction_component = entity->get_component<ces_ui_interaction_component>();
            switch (ui_interaction_component->get_ui())
            {
                case ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_left_label:
                {
                    m_tutorial_steer_left_label = entity;
                }
                break;
                
                case ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_right_label:
                {
                    m_tutorial_steer_right_label = entity;
                }
                break;
                
                default:
                break;
            }
        });
        
        if (!m_level.expired() && !m_main_car.expired())
        {
            const auto level_descriptor_component = m_level.lock()->get_component<ces_level_descriptor_component>();
            if (level_descriptor_component->is_started &&
                !level_descriptor_component->is_paused &&
                !level_descriptor_component->is_win &&
                !level_descriptor_component->is_loose)
            {
                const auto level_tutorial_component = m_level.lock()->get_component<ces_level_tutorial_component>();
                if (level_tutorial_component->is_active)
                {
                    const auto level_route_component = m_level.lock()->get_component<ces_level_route_component>();
                    std::vector<glm::vec2> route = level_route_component->route;
                    
                    ces_level_tutorial_component::e_tutorial_id tutorial_id = level_tutorial_component->id;
                    if (tutorial_id == ces_level_tutorial_component::e_tutorial_id_steer)
                    {
                        const auto car = std::static_pointer_cast<gb::game_object_3d>(m_main_car.lock());
                        glm::vec3 car_position = car->position;
                        
                        i32 nearest_next_checkpoint_index = 0;
                        f32 nearest_next_checkpoint_distance = glm::distance(glm::vec2(car_position.x, car_position.z), route.at(nearest_next_checkpoint_index));
                        
                        i32 index = 0;
                        for (auto route_it : route)
                        {
                            f32 distance = glm::distance(glm::vec2(car_position.x, car_position.z), route_it);
                            if (distance < nearest_next_checkpoint_distance)
                            {
                                nearest_next_checkpoint_distance = distance;
                                nearest_next_checkpoint_index = index;
                            }
                            index++;
                        }
                        
                        i32 near_checkpoint_index = (nearest_next_checkpoint_index + 1) % route.size();
                        i32 mid_checkpoint_index = (nearest_next_checkpoint_index + 2) % route.size();
                        i32 far_checkpoint_index = (nearest_next_checkpoint_index + 3) % route.size();
                        
                        const auto near_checkpoint_position = route.at(near_checkpoint_index);
                        const auto mid_checkpoint_position = route.at(mid_checkpoint_index);
                        const auto far_checkpoint_position = route.at(far_checkpoint_index);
                        
                        auto corner_angle = glm::get_angle_abc(glm::vec2(car_position.x, car_position.z), near_checkpoint_position, mid_checkpoint_position);
                        
#if defined(__IOS__)
                        
                        m_tutorial_steer_left_label.lock()->visible = corner_angle > 0.f && corner_angle < 90.f;
                        m_tutorial_steer_right_label.lock()->visible = corner_angle < 0.f && corner_angle > -90.f;
                        
                        
#elif defined(__OSX__)
                        
                        
                        const auto tutorial_steer_label = ui_controls_helper::get_control(ces_ui_interaction_component::e_ui::e_ui_level_tutorial_steer_label);
                        if (tutorial_steer_label)
                        {
                            bool is_visible = tutorial_steer_label->visible;
                            bool should_be_visible = (corner_angle > 0.f && corner_angle < 90.f) || (corner_angle < 0.f && corner_angle > -90.f);
                            static f32 last_visible_timestamp = std::get_tick_count();
                            if (!is_visible && should_be_visible)
                            {
                                last_visible_timestamp = std::get_tick_count();
                            }
                            f32 current_timestamp = std::get_tick_count();
                            tutorial_steer_label->visible = should_be_visible || current_timestamp - last_visible_timestamp < 3000.f;
                            std::static_pointer_cast<gb::ui::textfield>(tutorial_steer_label)->set_visible_edges(false);
                        }
                        
#endif
                        std::static_pointer_cast<gb::ui::textfield>(m_tutorial_steer_left_label.lock())->set_visible_edges(false);
                        std::static_pointer_cast<gb::ui::textfield>(m_tutorial_steer_right_label.lock())->set_visible_edges(false);
                        
                        static i32 blinking = 63;
                        
                        
#if defined(__IOS__)
                        
                        
                        blinking += 20;
                        
#elif defined(__OSX__)
                        
                        
                        blinking += 5;
                        
#endif
                        
                        if (blinking >= 255)
                        {
                            blinking = 63;
                        }
                        if (!m_tutorial_steer_left_label.expired())
                        {
                            std::static_pointer_cast<gb::ui::textfield>(m_tutorial_steer_left_label.lock())->set_font_color(glm::u8vec4(255, 255, 255, blinking));
                        }
                        
                        if (!m_tutorial_steer_right_label.expired())
                        {
                            std::static_pointer_cast<gb::ui::textfield>(m_tutorial_steer_right_label.lock())->set_font_color(glm::u8vec4(255, 255, 255, blinking));
                        }
                        
#if defined(__OSX__)
                        
                        if (tutorial_steer_label)
                        {
                            std::static_pointer_cast<gb::ui::textfield>(tutorial_steer_label)->set_font_color(glm::u8vec4(255, 255, 255, blinking));
                        }
                        
#endif
                    }
                }
            }
        }
    }
    
    void ces_level_tutorial_system::on_feed_end(f32 dt)
    {
        
    }
}
