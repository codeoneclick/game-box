//
//  ui_animation_helper.cpp
//  gbDemo
//
//  Created by serhii.s on 6/12/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ui_animation_helper.h"
#include "ces_ui_interaction_component.h"

namespace game
{
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
    
    void ui_animation_helper::show_from_right(const gb::ui::control_shared_ptr& control, const glm::ivec2& screen_size, f32 show_progress)
    {
        if (control)
        {
            const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
            const auto target_position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ui_interaction_component->get_ui())];
            glm::vec2 position = control->position;
            glm::vec2 size = control->size;
            
            position.x = glm::mix(screen_size.x  + size.x, target_position.x, show_progress);
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
    
    void ui_animation_helper::show_from_down(const gb::ui::control_shared_ptr& control, const glm::ivec2& screen_size, f32 show_progress)
    {
        if (control)
        {
            const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
            const auto target_position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ui_interaction_component->get_ui())];
            glm::vec2 position = control->position;
            glm::vec2 size = control->size;
            
            position.y = glm::mix(screen_size.y  + size.y, target_position.y, show_progress);
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
    
    void ui_animation_helper::hide_to_right(const gb::ui::control_shared_ptr& control, const glm::ivec2& screen_size, f32 hide_progress)
    {
        if (control)
        {
            const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
            const auto target_position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ui_interaction_component->get_ui())];
            glm::vec2 position = control->position;
            glm::vec2 size = control->size;
            
            position.x = glm::mix(position.x, screen_size.x + size.x, hide_progress);
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
    
    void ui_animation_helper::hide_to_down(const gb::ui::control_shared_ptr& control, const glm::ivec2& screen_size, f32 hide_progress)
    {
        if (control)
        {
            const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
            const auto target_position = ces_ui_interaction_component::k_controls_position[static_cast<i32>(ui_interaction_component->get_ui())];
            glm::vec2 position = control->position;
            glm::vec2 size = control->size;
            
            position.y = glm::mix(position.y, screen_size.y + size.y, hide_progress);
            control->position = position;
        }
    }
}
