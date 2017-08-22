//
//  ces_character_statistic_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 6/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_character_statistic_component : public gb::ces_base_component
    {
    public:
        
        enum e_mode
        {
            e_mode_unknown = -1,
            e_mode_player = 0,
            e_mode_mob,
            e_mode_npc
        };
        
        enum e_parameter
        {
            e_parameter_hp = 0,
            e_parameter_damage,
            e_parameter_move_speed,
            e_parameter_attack_speed,
            e_parameter_attack_distance,
            e_parameter_chase_start_distance,
            e_parameter_chase_end_distance,
            e_parameter_reviving_time,
            e_parameters_max
        };
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&, e_parameter, f32)> on_parameter_changed_callback_t;
        
    private:
        
        std::vector<std::tuple<gb::ces_entity_weak_ptr, on_parameter_changed_callback_t>> m_on_parameter_changed_callbacks;
        void on_parameter_changed(e_parameter parameter, f32 delta);
        
        gb::ces_entity_weak_ptr m_health_status_entity;
        std::array<f32, e_parameters_max> m_initial_parameters;
        std::array<f32, e_parameters_max> m_current_parameters;
        
        glm::vec2 m_spawn_position;
        f32 m_spawn_distance_delta;
        std::chrono::steady_clock::time_point m_dead_timestamp;
        
        e_mode m_mode;
        
        i32 m_character_class_id;
        
    protected:
        
        
        
    public:
        
        CTTI_CLASS_GUID(ces_character_statistic_component, gb::ces_base_component::g_guids_container)
        ces_character_statistic_component();
        ~ces_character_statistic_component();
        
        void set_character_class_id(i32 id);
        i32 get_character_class_id() const;
        
        void set_base_parameters(f32 max_health,
                                 f32 max_move_speed,
                                 f32 max_attack_speed,
                                 f32 max_damage,
                                 f32 attack_distance,
                                 f32 reviving_time);
        
        void set_chase_paramaters(f32 chase_start_distance,
                                  f32 chase_end_distance);
        
        void set_health_status_entity(const gb::ces_entity_shared_ptr& entity);
        
        std::property_rw<f32> current_health;
        std::property_rw<f32> current_move_speed;
        std::property_rw<f32> current_attack_speed;
        std::property_rw<f32> current_damage;
        std::property_rw<f32> current_attack_distance;
        std::property_rw<f32> current_chase_start_distance;
        std::property_rw<f32> current_chase_end_distance;
        std::property_rw<f32> current_reviving_time;
        
        std::property_ro<f32> max_health;
        std::property_ro<f32> max_move_speed;
        std::property_ro<f32> max_attack_speed;
        std::property_ro<f32> max_damage;
        std::property_ro<f32> max_attack_distance;
        std::property_ro<f32> max_chase_start_distance;
        std::property_ro<f32> max_chase_end_distance;
        std::property_ro<f32> max_reviving_time;
        
        std::property_rw<e_mode> mode;
        
        void reset();
        
        std::property_ro<bool> is_dead;
        std::property_ro<f32> current_health_percents;
        
        gb::sprite_shared_ptr get_health_status_entity() const;
        
        void set_spawn_position(const glm::vec2& position, f32 delta = 0.f);
        glm::vec2 get_spawn_position() const;
        f32 get_spawn_delta() const;
        
        void set_dead_timestamp(std::chrono::steady_clock::time_point timestamp);
        std::chrono::steady_clock::time_point get_dead_timestamp() const;
        
        void add_on_parameter_changed_callback(const gb::ces_entity_shared_ptr& owner, const on_parameter_changed_callback_t& callback);
        void remove_on_parameter_changed_callback(const gb::ces_entity_shared_ptr& owner);
    };
};
