//
//  server_character_controller.h
//  gbDemo
//
//  Created by serhii serhiiv on 1/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "game_object.h"
#include "level.h"

namespace game
{
    class server_character_controller : public gb::game_object
    {
    public:
        
        typedef std::function<void(ui64, ui32, const glm::vec2&, const glm::vec2&, f32)>
        on_server_character_move_callback_t;
        typedef std::function<void(ui64, ui32, const glm::vec2&, const glm::vec2&, f32)>
        on_server_character_shoot_callback_t;
        
    private:
        
        struct client_character_move_timeprint
        {
            ui64 m_move_revision;
            f32 m_move_angle;
            f32 m_dt;
        };
        
        struct client_character_shoot_timeprint
        {
            ui64 m_shoot_revision;
            f32 m_shoot_angle;
        };
        
    protected:
        
        gb::scene_graph_weak_ptr m_scene_graph;
        gb::scene_fabricator_weak_ptr m_scene_fabricator;
        gb::anim::anim_fabricator_weak_ptr m_anim_fabricator;
        
        gb::game_object_shared_ptr m_character;
        std::array<gb::game_object_weak_ptr, level::e_level_layer_max> m_layers;
        i32 m_udid;
        
        glm::vec2 m_spawn_point;
        f32 m_dead_cooldown_timeinterval;
        std::chrono::steady_clock::time_point m_dead_timestamp;
        
        std::queue<client_character_move_timeprint> m_client_character_move_history;
        std::queue<client_character_shoot_timeprint> m_client_character_shoot_history;
        
        on_server_character_move_callback_t m_server_character_move_callback;
        on_server_character_shoot_callback_t m_server_character_shoot_callback;
        
        void on_health_changed(const gb::ces_entity_shared_ptr& entity, f32 health);
        void on_dead(const gb::ces_entity_shared_ptr& owner);
        void on_kill(const gb::ces_entity_shared_ptr& owner, const gb::ces_entity_shared_ptr& target);
        void on_revive();
        void on_shoot(const glm::vec2& position, f32 rotation, const glm::vec2& velocity);
        
    public:
        
        server_character_controller(ui32 udid,
                                    const gb::scene_graph_shared_ptr& scene_graph,
                                    const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                    const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                    const std::array<gb::game_object_weak_ptr, level::e_level_layer_max>& layers);
        ~server_character_controller();
        
        void setup(const std::string& filename);
        
        void set_spawn_point(const glm::vec2& spawn_point);
        
        void on_client_character_move(ui64 move_revision, f32 move_angle, f32 dt);
        void set_server_character_move_callback(const on_server_character_move_callback_t& callback);
        
        void on_client_character_shoot(ui64 shoot_revision, f32 shoot_angle);
        void set_server_character_shoot_callback(const on_server_character_shoot_callback_t& callback);
        
        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime);
    };
};
