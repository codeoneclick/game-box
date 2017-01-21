//
//  client_main_character_controller.h
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "client_base_character_controller.h"

namespace ns
{
    class client_main_character_controller : public client_base_character_controller
    {
    public:
        
        typedef std::function<void(const glm::vec2&, bool)> on_character_moving_callback_t;
        
    private:
        
    protected:
        
        gb::ui::joystick_shared_ptr m_joystick;
        gb::camera_shared_ptr m_camera;
        
        glm::vec2 m_joystick_delta;
        bool m_is_dragging;
        
        void on_joystick_dragging(const gb::ces_entity_shared_ptr& joystick, const glm::vec2& delta, f32 angle);
        void on_joystick_end_dragging(const gb::ces_entity_shared_ptr& joystick);
        
        on_character_moving_callback_t m_character_moving_callback;
        
    public:
        
        client_main_character_controller(const gb::camera_shared_ptr& camera, const gb::game_object_shared_ptr& character);
        ~client_main_character_controller();
        
        void set_joystick(const gb::ui::joystick_shared_ptr& joystick);
        void set_character_moving_callback(const on_character_moving_callback_t& callback);
        
        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime) override;
    };
};
