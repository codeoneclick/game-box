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
    ces_character_controllers_component::ces_character_controllers_component() :
    m_mode(e_mode::e_mode_unknown)
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
    }
    
    ces_character_controllers_component::~ces_character_controllers_component()
    {
        
    }
}
