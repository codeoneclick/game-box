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
    ces_character_statistic_component::ces_character_statistic_component() :
    m_max_health(0.f),
    m_max_move_speed(0.f),
    m_max_attack_speed(0.f),
    m_max_damage(0.f),
    m_current_health(0.f),
    m_current_move_speed(0.f),
    m_current_attack_speed(0.f),
    m_current_damage(0.f),
    m_on_health_changed_callback(nullptr)
    {
        current_health.setter([=](f32 health) {
            m_current_health = health;
        });
        
        current_health.getter([=]() {
            return m_current_health;
        });
        
        current_move_speed.setter([=](f32 speed) {
            m_current_move_speed = speed;
        });
        
        current_move_speed.getter([=]() {
            return m_current_move_speed;
        });
        
        current_attack_speed.setter([=](f32 speed) {
            m_current_attack_speed = speed;
        });
        
        current_attack_speed.getter([=]() {
            return m_current_attack_speed;
        });
        
        current_damage.setter([=](f32 damage) {
            m_current_damage = damage;
        });
        
        current_damage.getter([=]() {
            return m_current_damage;
        });
        
        
        max_health.getter([=]() {
            return m_max_health;
        });
        
        max_move_speed.getter([=]() {
            return m_max_move_speed;
        });
        
        max_attack_speed.getter([=]() {
            return m_max_attack_speed;
        });
        
        max_damage.getter([=]() {
            return m_max_damage;
        });
        
        is_dead.getter([=]() {
            return m_current_health <= 0.f;
        });
        
        current_health_percents.getter([=]() {
            return m_current_health / m_max_health;
        });
    }
    
    ces_character_statistic_component::~ces_character_statistic_component()
    {
        
    }
    
    void ces_character_statistic_component::setup(f32 max_health,
                                                  f32 max_move_speed,
                                                  f32 max_attack_speed,
                                                  f32 max_damage)
    {
        m_max_health = m_current_health = max_health;
        m_max_move_speed = m_current_move_speed = max_move_speed;
        m_max_attack_speed = m_current_attack_speed = max_attack_speed;
        m_max_damage = m_current_damage = max_damage;
    }
    
    void ces_character_statistic_component::on_health_changed(const gb::ces_entity_shared_ptr& owner, f32 delta)
    {
        m_current_health += delta;
        if(m_on_health_changed_callback)
        {
            m_on_health_changed_callback(owner, delta);
        }
    }
    
    void ces_character_statistic_component::set_on_health_changed_callback(const on_health_changed_callback_t& callback)
    {
        m_on_health_changed_callback = callback;
    }
}
