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
            const auto level_tutorial_component = m_level.lock()->get_component<ces_level_tutorial_component>();
            if (level_tutorial_component->is_active)
            {
                const auto level_route_component = m_level.lock()->get_component<ces_level_route_component>();
                std::vector<glm::vec2> route = level_route_component->route;
                
                ces_level_tutorial_component::e_tutorial_id tutorial_id = level_tutorial_component->id;
                if (tutorial_id == ces_level_tutorial_component::e_tutorial_id_steer)
                {
                    i32 step = level_tutorial_component->step;
                    const auto car_descriptor_component = m_main_car.lock()->get_component<ces_car_descriptor_component>();
                    i32 checkpoint_passed_index = car_descriptor_component->checkpoint_passed_index;
                    if (step == 0 && checkpoint_passed_index < 11)
                    {
                        const auto car_input_component = m_main_car.lock()->get_component<ces_car_input_component>();
                        if (car_input_component)
                        {
                            m_main_car.lock()->remove_component(car_input_component);
                            const auto car_ai_input_component = std::make_shared<ces_car_ai_input_component>();
                            m_main_car.lock()->add_component(car_ai_input_component);
                        }
                        
                        if (!m_tutorial_steer_left_label.expired())
                        {
                            m_tutorial_steer_left_label.lock()->visible = false;
                        }
                        
                        if (!m_tutorial_steer_right_label.expired())
                        {
                            m_tutorial_steer_right_label.lock()->visible = false;
                        }
                    }
                    else if (step == 0 && checkpoint_passed_index == 11)
                    {
                        root->get_component<gb::ces_box2d_world_component>()->set_update_interval(1.f / 240.f);
                        step = 1;
                        level_tutorial_component->step = step;
                        
                        const auto car_ai_input_component = m_main_car.lock()->get_component<ces_car_ai_input_component>();
                        if (car_ai_input_component)
                        {
                            m_main_car.lock()->remove_component(car_ai_input_component);
                            const auto car_input_component = std::make_shared<ces_car_input_component>();
                            m_main_car.lock()->add_component(car_input_component);
                        }
                        
                        if (!m_tutorial_steer_left_label.expired())
                        {
                            m_tutorial_steer_left_label.lock()->visible = true;
                            std::static_pointer_cast<gb::ui::textfield>(m_tutorial_steer_left_label.lock())->set_visible_edges(false);
                        }
                        
                        if (!m_tutorial_steer_right_label.expired())
                        {
                            m_tutorial_steer_right_label.lock()->visible = false;
                        }
                    }
                    else if (step == 1 && checkpoint_passed_index == 12)
                    {
                        if (!m_tutorial_steer_left_label.expired())
                        {
                            m_tutorial_steer_left_label.lock()->visible = false;
                        }
                        
                        if (!m_tutorial_steer_right_label.expired())
                        {
                            m_tutorial_steer_right_label.lock()->visible = false;
                        }
                    }
                    else if (step == 1 && checkpoint_passed_index == 13)
                    {
                        root->get_component<gb::ces_box2d_world_component>()->set_update_interval(1.f / 240.f);
                        step = 2;
                        level_tutorial_component->step = step;
                        
                        if (!m_tutorial_steer_left_label.expired())
                        {
                            m_tutorial_steer_left_label.lock()->visible = false;
                        }
                        
                        if (!m_tutorial_steer_right_label.expired())
                        {
                            m_tutorial_steer_right_label.lock()->visible = true;
                            std::static_pointer_cast<gb::ui::textfield>(m_tutorial_steer_right_label.lock())->set_visible_edges(false);
                        }
                    }
                    else if (step == 2 && checkpoint_passed_index == 14)
                    {
                        if (!m_tutorial_steer_left_label.expired())
                        {
                            m_tutorial_steer_left_label.lock()->visible = false;
                        }
                        
                        if (!m_tutorial_steer_right_label.expired())
                        {
                            m_tutorial_steer_right_label.lock()->visible = false;
                        }
                    }
                    else if (step == 2 && checkpoint_passed_index == 15)
                    {
                        root->get_component<gb::ces_box2d_world_component>()->set_update_interval(1.f / 240.f);
                        step = 3;
                        level_tutorial_component->step = step;
                        if (!m_tutorial_steer_left_label.expired())
                        {
                            m_tutorial_steer_left_label.lock()->visible = true;
                            std::static_pointer_cast<gb::ui::textfield>(m_tutorial_steer_left_label.lock())->set_visible_edges(false);
                        }
                        
                        if (!m_tutorial_steer_right_label.expired())
                        {
                            m_tutorial_steer_right_label.lock()->visible = false;
                        }
                    }
                    else if (step == 3 && checkpoint_passed_index == 16)
                    {
                        step = 4;
                        level_tutorial_component->step = step;
                        
                        if (!m_tutorial_steer_left_label.expired())
                        {
                            m_tutorial_steer_left_label.lock()->visible = false;
                        }
                        
                        if (!m_tutorial_steer_right_label.expired())
                        {
                            m_tutorial_steer_right_label.lock()->visible = false;
                        }
                    }
                    else if (step == 4)
                    {
                        auto current_box2d_update_interval = root->get_component<gb::ces_box2d_world_component>()->get_update_interval();
                        static ui64 tutorial_steer_end_timestamp = std::get_tick_count();
                        ui64 current_time = std::get_tick_count();
                        ui64 delta_time = current_time - tutorial_steer_end_timestamp;
                        current_box2d_update_interval = glm::mix(current_box2d_update_interval, 1.f / 60.f, static_cast<f32>(delta_time) / 1000.f);
                        root->get_component<gb::ces_box2d_world_component>()->set_update_interval(current_box2d_update_interval);
                        
                        if (delta_time > 1000)
                        {
                            root->get_component<gb::ces_box2d_world_component>()->set_update_interval(1.f / 60.f);
                            step = 5;
                            level_tutorial_component->step = step;
                            level_tutorial_component->is_active = false;
                        }
                    }
                    
                    static i32 blinking = 63;
                    blinking += 10;
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
                }
            }
        }
    }
    
    void ces_level_tutorial_system::on_feed_end(f32 dt)
    {
        
    }
}
