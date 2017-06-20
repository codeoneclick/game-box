//
//  client_base_character_controller.h
//  gbDemo
//
//  Created by serhii serhiiv on 1/20/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "ns_declarations.h"
#include "game_object_2d.h"
#include "level.h"

namespace game
{
    class client_base_character_controller : public gb::game_object_2d
    {
    public:
        
        typedef std::function<void(const std::string&)> statistic_callback_t;
        static const std::string k_character_statistic_part;
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&)> on_tap_on_character_callback_t;
        
    private:
        
        static const std::string k_character_statistic_entity_filename;
        statistic_callback_t m_statistic_callback;
        
    protected:
        
        gb::scene_graph_weak_ptr m_scene_graph;
        gb::scene_fabricator_weak_ptr m_scene_fabricator;
        gb::anim::anim_fabricator_weak_ptr m_anim_fabricator;
        
        gb::game_object_2d_shared_ptr m_character;
        gb::game_object_2d_shared_ptr m_character_statistic;
        std::array<gb::game_object_2d_weak_ptr, level::e_level_layer_max> m_layers;
        
        information_bubble_controller_weak_ptr m_information_bubble_controller;
        bloodprint_controller_weak_ptr m_bloodprint_controller;
        footprint_controller_weak_ptr m_footprint_controller;
        
        std::chrono::steady_clock::time_point m_footprint_previous_timestamp;
        
        glm::vec2 m_server_velocity;
        glm::vec2 m_server_position;
        f32 m_server_rotation;
        
        glm::vec2 m_spawn_point;
        f32 m_dead_cooldown_timeinterval;
        std::chrono::steady_clock::time_point m_dead_timestamp;
        
        on_tap_on_character_callback_t m_on_tap_on_character_callback;
        
        virtual void update(const gb::ces_entity_shared_ptr& entity, f32 dt);
        
        void on_health_updated();
        
        virtual void on_move();
        virtual void on_idle();
        
        void create_hit_bounds();
        
        virtual void on_health_changed(const gb::ces_entity_shared_ptr& entity, f32 health);
        virtual void on_died(const gb::ces_entity_shared_ptr& owner);
        virtual void on_killed(const gb::ces_entity_shared_ptr& owner, const gb::ces_entity_shared_ptr& target);
        virtual void on_revive();
        
        virtual void on_animation_ended(const std::string& animation_name, bool is_looped);
        void on_death_effect_ended();
        
        void setup_controllers();
        
    public:
        
        client_base_character_controller(const gb::scene_graph_shared_ptr& scene_graph,
                                         const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                         const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                         const std::array<gb::game_object_2d_weak_ptr, level::e_level_layer_max>& layers);
        ~client_base_character_controller();
        
        virtual void setup(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage);
        
        void on_changed_server_transformation(const glm::vec2& velocity,
                                              const glm::vec2& position,
                                              f32 rotation);
        
        void set_spawn_point(const glm::vec2& spawn_point);
        
        void set_statistic_callback(const statistic_callback_t& callback);
        void set_on_tap_on_character_callback(const on_tap_on_character_callback_t& callback);
    };
};
