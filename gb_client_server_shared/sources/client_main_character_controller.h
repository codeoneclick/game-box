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
        
        typedef std::function<void(i32, i32)> on_dead_cooldown_callback_t;
        
    private:
        
        on_dead_cooldown_callback_t m_dead_cooldown_callback;

		bool m_is_net_session;
        
        path_map_shared_ptr m_path_map;
        pathfinder_shared_ptr m_pathfinder;
        std::queue<glm::vec2> m_move_path;
        
        ai_actions_processor_shared_ptr m_actions_processor;
        
        bool m_is_locked_on_attack;
        
        void on_move_action_callback(const ai_action_shared_ptr& action);
        void on_attack_action_callback(const ai_action_shared_ptr& action);
        void on_attack_animation_end_callback(const std::string& animation_name, bool is_looped);
        
    protected:
        
        gb::camera_2d_shared_ptr m_camera;
        glm::vec2 m_map_size;
        
        gb::ces_entity_weak_ptr m_selected_character_entity;
        
		void on_attack();
    
        void update(const gb::ces_entity_shared_ptr& entity, f32 deltatime) override;
        
    public:
        
        client_main_character_controller(bool is_net_session,
										 const gb::camera_2d_shared_ptr& camera,
                                         const gb::scene_graph_shared_ptr& scene_graph,
                                         const gb::scene_fabricator_shared_ptr& scene_fabricator,
                                         const gb::anim::anim_fabricator_shared_ptr& anim_fabricator,
                                         const std::array<gb::game_object_2d_weak_ptr, level::e_level_layer_max>& layers);
        ~client_main_character_controller();
        
        void setup(const std::pair<gb::sprite_shared_ptr, gb::shape_3d_shared_ptr>& character_linkage) override;
        
        void set_map_size(const glm::vec2& map_size);
        
        void set_dead_cooldown_callback(const on_dead_cooldown_callback_t& callback);
        
        void set_path_map(const path_map_shared_ptr& path_map);
        
        void on_tap_on_level_at_position(const glm::vec2& position);
        void on_tap_on_character(const gb::ces_entity_shared_ptr& entity) override;
        void on_tap_on_attack_button(const gb::ces_entity_shared_ptr&);
        void on_tap_on_ability_1_button(const gb::ces_entity_shared_ptr&);
        void on_tap_on_ability_2_button(const gb::ces_entity_shared_ptr&);
        void on_tap_on_ability_3_button(const gb::ces_entity_shared_ptr&);
    };
};
