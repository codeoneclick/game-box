//
//  ces_ui_animation_action_component.h
//  gbDemo
//
//  Created by serhii.s on 7/31/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_action_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_ui_move_animation_action_component : public gb::ces_action_component
    {
    public:
        
        enum class e_move_direction
        {
            e_undefined = 0,
            e_left,
            e_right,
            e_top,
            e_down
        };
        
        enum class e_move_mode
        {
            e_undefined = 0,
            e_show,
            e_hide
        };
        
    private:
        
        f32 m_progress = 0.f;
        f32 m_duration_in_seconds = 0.f;
        f32 m_current_duration_in_seconds = 0.f;
        e_move_direction m_move_direction = e_move_direction::e_undefined;
        e_move_mode m_move_mode = e_move_mode::e_undefined;
        bool m_is_finished = false;
        
    protected:
        
    public:
        
        f32 get_progress() const;
        bool get_is_finished() const;
        
        void set_duration_in_second(f32 duration);
        void set_move_direction(e_move_direction direction);
        void set_move_mode(e_move_mode mode);
        
        void play(const std::function<void(const gb::ces_entity_shared_ptr&, f32 dt, e_move_direction, e_move_mode, f32)>& callback);
    };
};
