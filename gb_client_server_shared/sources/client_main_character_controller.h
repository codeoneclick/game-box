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
    class client_main_character_move_history_point
    {
    private:
        
        ui64 m_timestamp;
        glm::vec2 m_position;
        f32 m_rotation;
        
    protected:
        
    public:
        
        client_main_character_move_history_point(ui64 timestamp, const glm::vec2& position, f32 rotation);
        
        ui64 get_timestamp() const;
        glm::vec2 get_position() const;
        f32 get_rotation() const;
    };
    
    class client_main_character_controller : public client_base_character_controller
    {
    public:
        
        typedef std::function<void(ui64, const glm::vec2&, bool)> on_character_moving_callback_t;
        
    private:
        
        ui64 m_timestamp;
        std::list<std::shared_ptr<client_main_character_move_history_point>> m_movement_history;
        on_character_moving_callback_t m_character_moving_callback;
        
    protected:
        
        gb::ui::joystick_shared_ptr m_joystick;
        gb::camera_shared_ptr m_camera;
        
        glm::vec2 m_joystick_delta;
        bool m_is_dragging;
        
        glm::vec2 m_server_adjust_position;
        f32 m_server_adjust_rotation;
        
        ui64 m_last_server_timestamp;
        
        void on_joystick_dragging(const gb::ces_entity_shared_ptr& joystick, const glm::vec2& delta, f32 angle);
        void on_joystick_end_dragging(const gb::ces_entity_shared_ptr& joystick);
        
        bool check_synchronization(ui64 timestamp, const glm::vec2& position, f32 rotation);
        
    public:
        
        client_main_character_controller(const gb::camera_shared_ptr& camera);
        ~client_main_character_controller();
        
        void set_joystick(const gb::ui::joystick_shared_ptr& joystick);
        void set_character_moving_callback(const on_character_moving_callback_t& callback);
        
        void set_character(const gb::game_object_shared_ptr& character) override;
        
        void synchronize_transformations(ui64 timestamp, const glm::vec2& position, const f32 rotation);
        
        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime) override;
        
        glm::vec2 get_position() const;
        f32 get_rotation() const;
    };
};
