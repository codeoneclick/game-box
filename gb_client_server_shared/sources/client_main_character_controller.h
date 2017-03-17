//
//  client_main_character_controller.h
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#pragma once

#include "client_base_character_controller.h"

namespace game
{
    class client_main_character_controller : public client_base_character_controller
    {
    public:
        
        typedef std::function<void(ui64, const glm::vec2&)> on_character_moving_callback_t;
        
    private:
        
        struct client_character_move_history_point
        {
            ui64 m_client_tick;
            glm::vec2 m_position;
            f32 m_rotation;
        };
        
        ui64 m_client_tick;
        std::list<client_character_move_history_point> m_client_character_move_history;
        on_character_moving_callback_t m_character_moving_callback;

		bool m_is_net_session;
        
    protected:
        
        gb::ui::fullscreen_joystick_shared_ptr m_joystick;
        gb::camera_shared_ptr m_camera;
        glm::vec2 m_spawn_point;
        
        glm::vec2 m_joystick_delta;
        bool m_is_dragging;
        
        glm::vec2 m_server_adjust_position;
        f32 m_server_adjust_rotation;
        
        ui64 m_received_client_tick;
        
        void on_joystick_dragging(const gb::ces_entity_shared_ptr& joystick, const glm::vec2& delta, f32 angle);
        void on_joystick_end_dragging(const gb::ces_entity_shared_ptr& joystick);

		void on_shoot_button_pressed(const gb::ces_entity_shared_ptr& entity);
        
        bool check_synchronization(ui64 client_tick, const glm::vec2& position, f32 rotation);
        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime) override;
        
    public:
        
        client_main_character_controller(bool is_net_session,
										 const gb::camera_shared_ptr& camera,
                                         const gb::game_object_shared_ptr& layer,
                                         const gb::scene_graph_shared_ptr& scene_graph,
                                         const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                         const gb::anim::anim_fabricator_shared_ptr& anim_fabricator);
        ~client_main_character_controller();
        
        void set_joystick(const gb::ui::fullscreen_joystick_shared_ptr& joystick);
        void set_character_moving_callback(const on_character_moving_callback_t& callback);
        void set_spawn_point(const glm::vec2& spawn_point);
        
        void synchronize_transformations(ui64 client_tick, const glm::vec2& position, const f32 rotation);
    };
};
