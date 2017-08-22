//
//  ces_character_controllers_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 10/31/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "ces_character_controllers_component.h"

namespace game
{
    ces_character_controllers_component::ces_character_controllers_component()
    {
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
        
        hit_bounds_controller.setter([=](const hit_bounds_controller_shared_ptr& hit_bounds_controller) {
            m_hit_bounds_controller = hit_bounds_controller;
        });
        
        hit_bounds_controller.getter([=]() -> hit_bounds_controller_shared_ptr {
            return m_hit_bounds_controller.lock();
        });
    }
    
    ces_character_controllers_component::~ces_character_controllers_component()
    {
        
    }
}
