//
//  ces_character_statistic_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 6/14/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_character_statistic_component.h"

namespace game
{
    ces_character_statistic_component::ces_character_statistic_component()
    {
        m_initial_parameters.fill(0.f);
        m_current_parameters.fill(0.f);
        
        m_initial_parameters[e_parameter_chase_start_distance] = m_current_parameters[e_parameter_chase_start_distance] = std::numeric_limits<f32>::max();
        m_initial_parameters[e_parameter_chase_end_distance] = m_current_parameters[e_parameter_chase_end_distance] = std::numeric_limits<f32>::max();
        
        current_health.setter([=](f32 hp) {
            f32 delta = m_current_parameters[e_parameter_hp];
            m_current_parameters[e_parameter_hp] = hp;
            delta -= m_current_parameters[e_parameter_hp];
            ces_character_statistic_component::on_parameter_changed(e_parameter_hp, -delta);
        });
        
        current_health.getter([=]() {
            return m_current_parameters[e_parameter_hp];
        });
        
        current_move_speed.setter([=](f32 speed) {
            f32 delta = m_current_parameters[e_parameter_move_speed];
            m_current_parameters[e_parameter_move_speed] = speed;
            delta -= m_current_parameters[e_parameter_move_speed];
            ces_character_statistic_component::on_parameter_changed(e_parameter_move_speed, -delta);
        });
        
        current_move_speed.getter([=]() {
            return m_current_parameters[e_parameter_move_speed];
        });
        
        current_attack_speed.setter([=](f32 speed) {
            f32 delta = m_current_parameters[e_parameter_attack_speed];
            m_current_parameters[e_parameter_attack_speed] = speed;
            delta -= m_current_parameters[e_parameter_attack_speed];
            ces_character_statistic_component::on_parameter_changed(e_parameter_attack_speed, -delta);
        });
        
        current_attack_speed.getter([=]() {
            return m_current_parameters[e_parameter_attack_speed];
        });
        
        current_damage.setter([=](f32 damage) {
            f32 delta = m_current_parameters[e_parameter_damage];
            m_current_parameters[e_parameter_damage] = damage;
            delta -= m_current_parameters[e_parameter_damage];
            ces_character_statistic_component::on_parameter_changed(e_parameter_damage, -delta);
        });
        
        current_damage.getter([=]() {
            return m_current_parameters[e_parameter_damage];
        });
        
        current_attack_distance.setter([=](f32 attack_distance) {
            f32 delta = m_current_parameters[e_parameter_attack_distance];
            m_current_parameters[e_parameter_attack_distance] = attack_distance;
            delta -= m_current_parameters[e_parameter_attack_distance];
            ces_character_statistic_component::on_parameter_changed(e_parameter_attack_distance, -delta);
        });
        
        current_attack_distance.getter([=]() {
            return m_current_parameters[e_parameter_attack_distance];
        });
        
        current_chase_start_distance.setter([=](f32 current_chase_start_distance) {
            f32 delta = m_current_parameters[e_parameter_chase_start_distance];
            m_current_parameters[e_parameter_chase_start_distance] = current_chase_start_distance;
            delta -= m_current_parameters[e_parameter_chase_start_distance];
            ces_character_statistic_component::on_parameter_changed(e_parameter_chase_start_distance, -delta);
        });
        
        current_chase_start_distance.getter([=]() {
            return m_current_parameters[e_parameter_chase_start_distance];
        });
        
        current_chase_end_distance.setter([=](f32 current_chase_end_distance) {
            f32 delta = m_current_parameters[e_parameter_chase_end_distance];
            m_current_parameters[e_parameter_chase_end_distance] = current_chase_end_distance;
            delta -= m_current_parameters[e_parameter_chase_end_distance];
            ces_character_statistic_component::on_parameter_changed(e_parameter_chase_end_distance, -delta);
        });
        
        current_chase_end_distance.getter([=]() {
            return m_current_parameters[e_parameter_chase_end_distance];
        });
        
        max_health.getter([=]() {
            return m_initial_parameters[e_parameter_hp];
        });
        
        max_move_speed.getter([=]() {
            return m_initial_parameters[e_parameter_move_speed];
        });
        
        max_attack_speed.getter([=]() {
            return m_initial_parameters[e_parameter_attack_speed];
        });
        
        max_damage.getter([=]() {
            return m_initial_parameters[e_parameter_damage];
        });
        
        max_attack_distance.getter([=]() {
            return m_initial_parameters[e_parameter_attack_distance];
        });
        
        max_chase_start_distance.getter([=]() {
            return m_initial_parameters[e_parameter_chase_start_distance];
        });
        
        max_chase_end_distance.getter([=]() {
            return m_initial_parameters[e_parameter_chase_end_distance];
        });
        
        is_dead.getter([=]() {
            return m_current_parameters[e_parameter_hp] <= 0.f;
        });
        
        current_health_percents.getter([=]() {
            return std::max(std::max(m_current_parameters[e_parameter_hp], .0001f) / m_initial_parameters[e_parameter_hp], 0.f);
        });
    }
    
    ces_character_statistic_component::~ces_character_statistic_component()
    {
        
    }
    
    void ces_character_statistic_component::setup(f32 max_health,
                                                  f32 max_move_speed,
                                                  f32 max_attack_speed,
                                                  f32 max_damage,
                                                  f32 attack_distance)
    {
        m_initial_parameters[e_parameter_hp] = m_current_parameters[e_parameter_hp] = max_health;
        m_initial_parameters[e_parameter_move_speed] = m_current_parameters[e_parameter_move_speed] = max_move_speed;
        m_initial_parameters[e_parameter_attack_speed] = m_current_parameters[e_parameter_attack_speed] = max_attack_speed;
        m_initial_parameters[e_parameter_damage] = m_current_parameters[e_parameter_damage] = max_damage;
        m_initial_parameters[e_parameter_attack_distance] = m_current_parameters[e_parameter_attack_distance] = attack_distance;
    }
    
    void ces_character_statistic_component::setup(f32 chase_start_distance,
                                                  f32 chase_end_distance)
    {
        m_initial_parameters[e_parameter_chase_start_distance] = m_current_parameters[e_parameter_chase_start_distance] = chase_start_distance;
        m_initial_parameters[e_parameter_chase_end_distance] = m_current_parameters[e_parameter_chase_end_distance] = chase_end_distance;
    }
    
    void ces_character_statistic_component::on_parameter_changed(e_parameter parameter, f32 delta)
    {
        m_on_parameter_changed_callbacks.erase(std::remove_if(m_on_parameter_changed_callbacks.begin(), m_on_parameter_changed_callbacks.end(), [this, parameter, delta](const std::tuple<gb::ces_entity_weak_ptr, on_parameter_changed_callback_t>& it) {
            if(!std::get<0>(it).expired())
            {
                std::get<1>(it)(m_owner.lock(), parameter, delta);
                return false;
            }
            return true;
        }), m_on_parameter_changed_callbacks.end());
    }
    
    void ces_character_statistic_component::add_on_parameter_changed_callback(const gb::ces_entity_shared_ptr& owner, const on_parameter_changed_callback_t& callback)
    {
        assert(owner);
        assert(callback);
        m_on_parameter_changed_callbacks.push_back(std::make_tuple(owner, callback));
    }
    
    void ces_character_statistic_component::remove_on_parameter_changed_callback(const gb::ces_entity_shared_ptr& owner)
    {
        m_on_parameter_changed_callbacks.erase(std::remove_if(m_on_parameter_changed_callbacks.begin(), m_on_parameter_changed_callbacks.end(), [owner](const std::tuple<gb::ces_entity_weak_ptr, on_parameter_changed_callback_t>& it) {
            if(!std::get<0>(it).expired())
            {
                return owner == std::get<0>(it).lock();
            }
            return true;
        }), m_on_parameter_changed_callbacks.end());
    }
}
