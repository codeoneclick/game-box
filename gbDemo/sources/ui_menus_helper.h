//
//  ui_menus_helper.hpp
//  gbDemo
//
//  Created by serhii.s on 7/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "control.h"
#include "ui_controls_helper.h"
#include "ui_animation_helper.h"
#include "ces_ui_interaction_component.h"

namespace game
{
    class ui_menus_helper
    {
    private:
        
    protected:
        
    public:
        
        static void create_main_menu_ui(const gb::ces_entity_shared_ptr& root,
                                        const gameplay_ui_fabricator_shared_ptr& gameplay_ui_fabricator,
                                        bool hidden = false);
        
        static void create_garage_menu_ui(const gb::ces_entity_shared_ptr& root,
                                          const gameplay_ui_fabricator_shared_ptr& gameplay_ui_fabricator,
                                          bool hidden = false);
        
        static void create_in_game_menu_ui(const gb::ces_entity_shared_ptr& root,
                                           const gameplay_ui_fabricator_shared_ptr& gameplay_ui_fabricator,
                                           bool hidden = false);
        
        static void show_body_paint_ui();
        static void show_windshield_paint_ui();
        static void show_upgrade_performance_ui();
        static void hide_customization_ui();
        static void enable_customization_ui();
        static void disable_customization_ui();
        
        static void fill_selected_car_in_garage_ui(const gb::ces_entity_shared_ptr& root, const gb::ces_entity_shared_ptr& car);
    };
};

