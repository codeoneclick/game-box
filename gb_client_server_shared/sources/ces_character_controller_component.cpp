//
//  ces_character_controller_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_character_controller_component.h"

namespace game
{
    ces_character_controller_component::ces_character_controller_component() :
    m_mode(e_mode::unknown),
    m_health(100.f),
    m_spawn_callback(nullptr),
    m_health_changed_callback(nullptr)
    {
        mode.setter([=](e_mode mode) {
            m_mode = mode;
        });
        
        mode.getter([=]() {
            return m_mode;
        });
        
        health.getter([=]() {
            return m_health;
        });
    }
    
    ces_character_controller_component::~ces_character_controller_component()
    {
        
    }
    
    void ces_character_controller_component::add_health(const gb::ces_entity_shared_ptr& entity, f32 health)
    {
        m_health += health;
        if(m_health_changed_callback)
        {
            m_health_changed_callback(entity, health);
        }
    }
    
    void ces_character_controller_component::on_spawn(const gb::ces_entity_shared_ptr& entity)
    {
        if(m_spawn_callback)
        {
            m_spawn_callback(entity);
        }
        m_health = 100.f;
    }
    
    void ces_character_controller_component::set_spawn_callback(const spawn_callback_t& callback)
    {
        m_spawn_callback = callback;
    }
    
    void ces_character_controller_component::set_health_changed_callback(const health_changed_callback_t& callback)
    {
        m_health_changed_callback = callback;
    }
}
