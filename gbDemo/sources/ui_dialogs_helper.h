//
//  ui_dialogs_helper.hpp
//  gbDemo
//
//  Created by serhii.s on 7/31/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "control.h"
#include "ui_controls_helper.h"
#include "ui_animation_helper.h"
#include "ces_ui_interaction_component.h"

namespace game
{
    class ui_dialogs_helper
    {
    private:
        
        static gb::ces_entity_weak_ptr m_pushed_dialog;
        
        static void push_main_menu_button_labels();
        static void pop_main_menu_button_labels();
        
        static void push_shop_dialog(const gb::ces_entity_shared_ptr& root);
        static void pop_shop_dialog();
        
        static void push_career_dialog(const gb::ces_entity_shared_ptr& root);
        static void pop_career_dialog();
        
        static void push_quit_game_dialog(const gb::ces_entity_shared_ptr& root);
        static void pop_quit_game_dialog();
        
        static void push_cash_shop_dialog(const gb::ces_entity_shared_ptr& root);
        static void pop_cash_shop_dialog();
        
        static void push_garage_button_labels();
        static void pop_garage_button_labels();
        
        static void push_body_paint_dialog(const gb::ces_entity_shared_ptr& root);
        static void pop_body_paint_dialog();
        
        static void push_windshield_paint_dialog(const gb::ces_entity_shared_ptr& root);
        static void pop_windshield_paint_dialog();
        
        static void push_performance_upgrade_dialog(const gb::ces_entity_shared_ptr& root);
        static void pop_performance_upgrade_dialog();
        
        static void push_unlock_car_dialog(const gb::ces_entity_shared_ptr& root);
        static void pop_unlock_car_dialog();
        
    protected:
        
    public:
        
        static void push_dialog(ces_ui_interaction_component::e_ui ui_id, const gb::ces_entity_shared_ptr& root);
        static void pop_dialog();
        
        static gb::ces_entity_shared_ptr get_pushed_dialog();
    };
};
