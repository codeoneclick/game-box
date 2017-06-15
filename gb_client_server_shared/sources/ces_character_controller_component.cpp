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
    m_dead_callback(nullptr),
    m_kill_callback(nullptr)
    {
        mode.setter([=](e_mode mode) {
            m_mode = mode;
        });
        
        mode.getter([=]() {
            return m_mode;
        });
        
        information_bubble_controller.setter([=](const information_bubble_controller_shared_ptr& information_bubble_controller) {
            m_information_bubble_controller = information_bubble_controller;
        });
        
        information_bubble_controller.getter([=]() {
            return m_information_bubble_controller.lock();
        });
        
        bloodprint_controller.setter([=](const bloodprint_controller_shared_ptr& bloodprint_controller) {
            m_bloodprint_controller = bloodprint_controller;
        });
        
        bloodprint_controller.getter([=]() {
            return m_bloodprint_controller.lock();
        });

        footprint_controller.setter([=](const footprint_controller_shared_ptr& footprint_controller) {
            m_footprint_controller = footprint_controller;
        });
        
        footprint_controller.getter([=]() {
            return m_footprint_controller.lock();
        });
        
        character_statistic.setter([=](const gb::game_object_2d_shared_ptr& character_statistic) {
            m_character_statistic = character_statistic;
        });
        
        character_statistic.getter([=]() {
            return m_character_statistic.lock();
        });
    }
    
    ces_character_controller_component::~ces_character_controller_component()
    {
        
    }
    
    void ces_character_controller_component::on_dead(const gb::ces_entity_shared_ptr& owner)
    {
        if(m_dead_callback)
        {
            m_dead_callback(owner);
        }
    }
    
    void ces_character_controller_component::set_dead_callback(const dead_callback_t& callback)
    {
        m_dead_callback = callback;
    }

    void ces_character_controller_component::on_kill(const gb::ces_entity_shared_ptr& owner, const gb::ces_entity_shared_ptr& target)
    {
        if(m_kill_callback)
        {
            m_kill_callback(owner, target);
        }
    }
    
    void ces_character_controller_component::set_kill_callback(const kill_callback_t& callback)
    {
        m_kill_callback = callback;
    }
}
