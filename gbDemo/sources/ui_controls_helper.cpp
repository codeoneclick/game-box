//
//  ui_controls_helper.cpp
//  gbDemo
//
//  Created by serhii.s on 6/12/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ui_controls_helper.h"

namespace game
{
    std::array<gb::ui::control_weak_ptr, static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_max)> ui_controls_helper::m_all_ui_controls;
    
    void ui_controls_helper::add_control(const gb::ces_entity_shared_ptr& control)
    {
        const auto ui_interaction_component = control->get_component<ces_ui_interaction_component>();
        m_all_ui_controls[static_cast<i32>(ui_interaction_component->get_ui())] = std::static_pointer_cast<gb::ui::control>(control);
    }
    
    gb::ui::control_shared_ptr ui_controls_helper::get_control(ces_ui_interaction_component::e_ui id)
    {
        gb::ui::control_shared_ptr control = nullptr;
        if (!m_all_ui_controls[static_cast<i32>(id)].expired())
        {
            control = m_all_ui_controls[static_cast<i32>(id)].lock();
        }
        return control;
    }
    
    void ui_controls_helper::clear_controls()
    {
        for (const auto& control : m_all_ui_controls)
        {
            if (!control.expired())
            {
                control.lock()->remove_from_parent();
            }
        }
    }
    
    void ui_controls_helper::disable_all_and_focus_on(const std::vector<ces_ui_interaction_component::e_ui>& controls, bool focus)
    {
        for (const auto& control : m_all_ui_controls)
        {
            if (!control.expired())
            {
                const auto ui_interaction_component = control.lock()->get_component<ces_ui_interaction_component>();
                if (ui_interaction_component->get_ui() != ces_ui_interaction_component::e_ui::e_ui_screen_overlay)
                {
                    control.lock()->disable(true);
                }
            }
        }
        
        for (const auto ui_id_it : controls)
        {
            if (!m_all_ui_controls.at(static_cast<i32>(ui_id_it)).expired())
            {
                m_all_ui_controls.at(static_cast<i32>(ui_id_it)).lock()->disable(false);
                m_all_ui_controls.at(static_cast<i32>(ui_id_it)).lock()->focus(focus, .33f);
            }
        }
    }
    
    void ui_controls_helper::enable_all_and_unfocus()
    {
        for (const auto& control : m_all_ui_controls)
        {
            if (!control.expired())
            {
                const auto ui_interaction_component = control.lock()->get_component<ces_ui_interaction_component>();
                if (ui_interaction_component->get_ui() != ces_ui_interaction_component::e_ui::e_ui_screen_overlay)
                {
                    control.lock()->focus(false);
                    control.lock()->disable(false);
                }
            }
        }
    }
}
