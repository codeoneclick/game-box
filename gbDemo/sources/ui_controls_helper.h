//
//  ui_controls_helper.hpp
//  gbDemo
//
//  Created by serhii.s on 6/12/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "control.h"
#include "ces_ui_interaction_component.h"

namespace game
{
    class ui_controls_helper
    {
    private:
        
        static std::array<gb::ui::control_weak_ptr, static_cast<i32>(ces_ui_interaction_component::e_ui::e_ui_max)> m_all_ui_controls;
        
    protected:
        
    public:
        
        static void add_control(const gb::ces_entity_shared_ptr& control);
        static gb::ui::control_shared_ptr get_control(ces_ui_interaction_component::e_ui id);
        
        static void clear_controls();
        
        template<typename T>
        static std::shared_ptr<T> get_control_as(ces_ui_interaction_component::e_ui id)
        {
            const auto control = get_control(id);
            if (control)
            {
                return std::static_pointer_cast<T>(control);
            }
            else
            {
                return nullptr;
            }
        };
    };
};
