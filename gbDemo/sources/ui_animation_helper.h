//
//  ui_animation_helper.h
//  gbDemo
//
//  Created by serhii.s on 6/12/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "control.h"

namespace game
{
    class ui_animation_helper
    {
    private:
        
    protected:
        
    public:
      
        static void show_from_left(const gb::ui::control_shared_ptr& control, f32 show_progress);
        static void show_from_right(const gb::ui::control_shared_ptr& control, const glm::ivec2& screen_size, f32 show_progress);
        static void show_from_up(const gb::ui::control_shared_ptr& control, f32 show_progress);
        static void show_from_down(const gb::ui::control_shared_ptr& control, const glm::ivec2& screen_size, f32 show_progress);
        
        static void hide_to_left(const gb::ui::control_shared_ptr& control, f32 hide_progress);
        static void hide_to_right(const gb::ui::control_shared_ptr& control, const glm::ivec2& screen_size, f32 hide_progress);
        static void hide_to_up(const gb::ui::control_shared_ptr& control, f32 hide_progress);
        static void hide_to_down(const gb::ui::control_shared_ptr& control, const glm::ivec2& screen_size, f32 hide_progress);
    };
};
