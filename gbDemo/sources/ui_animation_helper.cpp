//
//  ui_animation_helper.cpp
//  gbDemo
//
//  Created by serhii.s on 6/12/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ui_animation_helper.h"
#include "ces_ui_interaction_component.h"
#include "ces_ui_animation_action_component.h"

namespace game
{
    glm::ivec2 ui_animation_helper::m_screen_size;
    void ui_animation_helper::set_screen_size(const glm::ivec2& size)
    {
        m_screen_size = size;
    }
    
    void ui_animation_helper::show_from_left(const gb::ui::control_shared_ptr& control, f32 show_progress)
    {
        if (control)
        {
            const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
            const auto target_position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ui_interaction_component->get_ui())];
            glm::vec2 position = control->position;
            glm::vec2 size = control->size;
            
            position.x = glm::mix(-size.x, target_position.x, show_progress);
            control->position = position;
        }
    }
    
    void ui_animation_helper::show_from_right(const gb::ui::control_shared_ptr& control, f32 show_progress)
    {
        if (control)
        {
            const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
            const auto target_position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ui_interaction_component->get_ui())];
            glm::vec2 position = control->position;
            glm::vec2 size = control->size;
            
            position.x = glm::mix(m_screen_size.x  + size.x, target_position.x, show_progress);
            control->position = position;
        }
    }
    
    void ui_animation_helper::show_from_up(const gb::ui::control_shared_ptr& control, f32 show_progress)
    {
        if (control)
        {
            const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
            const auto target_position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ui_interaction_component->get_ui())];
            glm::vec2 position = control->position;
            glm::vec2 size = control->size;
            
            position.y = glm::mix(-size.y, target_position.y, show_progress);
            control->position = position;
        }
    }
    
    void ui_animation_helper::show_from_down(const gb::ui::control_shared_ptr& control, f32 show_progress)
    {
        if (control)
        {
            const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
            const auto target_position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ui_interaction_component->get_ui())];
            glm::vec2 position = control->position;
            glm::vec2 size = control->size;
            
            position.y = glm::mix(m_screen_size.y  + size.y, target_position.y, show_progress);
            control->position = position;
        }
    }
    
    void ui_animation_helper::hide_to_left(const gb::ui::control_shared_ptr& control, f32 hide_progress)
    {
        if (control)
        {
            const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
            const auto target_position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ui_interaction_component->get_ui())];
            glm::vec2 position = control->position;
            glm::vec2 size = control->size;
            
            position.x = glm::mix(position.x, -size.x, hide_progress);
            control->position = position;
        }
    }
    
    void ui_animation_helper::hide_to_right(const gb::ui::control_shared_ptr& control, f32 hide_progress)
    {
        if (control)
        {
            const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
            const auto target_position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ui_interaction_component->get_ui())];
            glm::vec2 position = control->position;
            glm::vec2 size = control->size;
            
            position.x = glm::mix(position.x, m_screen_size.x + size.x, hide_progress);
            control->position = position;
        }
    }
    
    void ui_animation_helper::hide_to_up(const gb::ui::control_shared_ptr& control, f32 hide_progress)
    {
        if (control)
        {
            const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
            const auto target_position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ui_interaction_component->get_ui())];
            glm::vec2 position = control->position;
            glm::vec2 size = control->size;
            
            position.y = glm::mix(position.y, -size.y, hide_progress);
            control->position = position;
        }
    }
    
    void ui_animation_helper::hide_to_down(const gb::ui::control_shared_ptr& control, f32 hide_progress)
    {
        if (control)
        {
            const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
            const auto target_position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ui_interaction_component->get_ui())];
            glm::vec2 position = control->position;
            glm::vec2 size = control->size;
            
            position.y = glm::mix(position.y, m_screen_size.y + size.y, hide_progress);
            control->position = position;
        }
    }
    
    void ui_animation_helper::move_with_animation_action(const gb::ui::control_shared_ptr& control)
    {
        auto ui_move_animation_action_component = control->get_component<ces_ui_move_animation_action_component>();
        ui_move_animation_action_component->play([](const gb::ces_entity_shared_ptr & entity, f32 dt, ces_ui_move_animation_action_component::e_move_direction direction, ces_ui_move_animation_action_component::e_move_mode mode, f32 progress) {
            if (mode == ces_ui_move_animation_action_component::e_move_mode::e_show)
            {
                if (direction == ces_ui_move_animation_action_component::e_move_direction::e_right)
                {
                    show_from_left(std::static_pointer_cast<gb::ui::control>(entity), progress);
                }
                if (direction == ces_ui_move_animation_action_component::e_move_direction::e_left)
                {
                    show_from_right(std::static_pointer_cast<gb::ui::control>(entity), progress);
                }
                if (direction == ces_ui_move_animation_action_component::e_move_direction::e_down)
                {
                    show_from_up(std::static_pointer_cast<gb::ui::control>(entity), progress);
                }
                if (direction == ces_ui_move_animation_action_component::e_move_direction::e_top)
                {
                    show_from_down(std::static_pointer_cast<gb::ui::control>(entity), progress);
                }
            }
            else if (mode == ces_ui_move_animation_action_component::e_move_mode::e_hide)
            {
                if (direction == ces_ui_move_animation_action_component::e_move_direction::e_right)
                {
                    hide_to_right(std::static_pointer_cast<gb::ui::control>(entity), progress);
                }
                if (direction == ces_ui_move_animation_action_component::e_move_direction::e_left)
                {
                    hide_to_left(std::static_pointer_cast<gb::ui::control>(entity), progress);
                }
                if (direction == ces_ui_move_animation_action_component::e_move_direction::e_down)
                {
                    hide_to_down(std::static_pointer_cast<gb::ui::control>(entity), progress);
                }
                if (direction == ces_ui_move_animation_action_component::e_move_direction::e_top)
                {
                    hide_to_up(std::static_pointer_cast<gb::ui::control>(entity), progress);
                }
            }
            
            if (entity->get_component<ces_ui_move_animation_action_component>()->get_is_finished())
            {
                entity->remove_component(ces_ui_move_animation_action_component::class_guid());
                if (mode == ces_ui_move_animation_action_component::e_move_mode::e_hide)
                {
                    entity->visible = false;
                }
            }
        });
    }
}
