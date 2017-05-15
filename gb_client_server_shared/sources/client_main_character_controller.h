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
        
        typedef std::function<void(ui64, f32, f32)> on_character_move_callback_t;
        typedef std::function<void(ui64, f32)> on_character_shoot_callback_t;
        typedef std::function<void(i32, i32)> on_dead_cooldown_callback_t;
        
    private:
        
        struct client_character_move_history_point
        {
            ui64 m_move_revision;
            glm::vec2 m_position;
            f32 m_rotation;
        };
        
        ui64 m_move_revision;
        std::list<client_character_move_history_point> m_client_character_move_history;
        
        ui64 m_shoot_revision;
        
        on_character_move_callback_t m_character_move_callback;
        on_character_shoot_callback_t m_character_shoot_callback;
        on_dead_cooldown_callback_t m_dead_cooldown_callback;

		bool m_is_net_session;
        bool m_is_move_interacted;
        bool m_is_shoot_interacted;
        
    protected:
        
        gb::ui::joystick_shared_ptr m_move_joystick;
        gb::ui::joystick_shared_ptr m_shoot_joystick;
        
        glm::vec2 m_move_joystick_delta;
        glm::vec2 m_shoot_joystick_delta;
        
        bool m_move_joystick_dragging;
        bool m_shoot_joystick_dragging;
        
        f32 m_move_joystick_angle;
        f32 m_shoot_joystick_angle;
        
        gb::camera_shared_ptr m_camera;
        
        glm::vec2 m_map_size;
        
        glm::vec2 m_server_adjust_position;
        f32 m_server_adjust_rotation;
        ui64 m_server_adjust_move_revision;
        
        void on_joystick_dragging(const gb::ces_entity_shared_ptr& joystick, const glm::vec2& delta, f32 angle);
        void on_joystick_end_dragging(const gb::ces_entity_shared_ptr& joystick);

		void on_shoot();
        
        bool validate_move_synchronization(ui64 move_revision, const glm::vec2& position, f32 rotation);

        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime) override;
        
    public:
        
        client_main_character_controller(bool is_net_session,
										 const gb::camera_shared_ptr& camera,
                                         const gb::scene_graph_shared_ptr& scene_graph,
                                         const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                         const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                         const std::array<gb::game_object_2d_weak_ptr, level::e_level_layer_max>& layers);
        ~client_main_character_controller();
        
        void set_character_move_callback(const on_character_move_callback_t& callback);
        void set_character_shoot_callback(const on_character_shoot_callback_t& callback);
        
        void set_map_size(const glm::vec2& map_size);
        
        void set_move_joystick(const gb::ui::joystick_shared_ptr& joystick);
        void set_shoot_joystick(const gb::ui::joystick_shared_ptr& joystick);
        
        void synchronize_transformations(ui64 move_revision, const glm::vec2& position, const f32 rotation);
        void set_dead_cooldown_callback(const on_dead_cooldown_callback_t& callback);
        
    };
};
