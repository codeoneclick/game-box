//
//  control.cpp
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "control.h"

namespace gb
{
    namespace ui
    {
        control::control(const scene_fabricator_shared_ptr& fabricator) :
        m_fabricator(fabricator)
        {
            
        }
        
        control::~control()
        {
            
        }
        
        scene_fabricator_shared_ptr control::get_fabricator() const
        {
            return m_fabricator.lock();
        }
        
        void control::set_element_horizontal_aligment(const game_object_shared_ptr& element,
                                                      e_element_horizontal_aligment aligment)
        {
            glm::vec4 container_frame = game_object::get_bound_frame();
            glm::vec4 element_frame = element->get_bound_frame();
            switch (aligment)
            {
                case e_element_horizontal_aligment_left:
                {
                    element->set_position(glm::vec2(0.f, element->get_position().y));
                }
                    break;
                    
                case e_element_horizontal_aligment_right:
                {
                    element->set_position(glm::vec2(container_frame.z - element_frame.z,
                                                    element->get_position().y));
                }
                    break;
                
                case e_element_horizontal_aligment_center:
                {
                    element->set_position(glm::vec2((container_frame.z - element_frame.z) * .5f,
                                                    element->get_position().y));
                }
                    break;
            }
        }
        
        void control::set_element_vertical_aligment(const game_object_shared_ptr& element,
                                                    e_element_vertical_aligment aligment)
        {
            glm::vec4 container_frame = game_object::get_bound_frame();
            glm::vec4 element_frame = element->get_bound_frame();
            switch (aligment)
            {
                case e_element_vertical_aligment_top:
                {
                    element->set_position(glm::vec2(element->get_position().x, 0.f));
                }
                    break;
                    
                case e_element_vertical_aligment_down:
                {
                    element->set_position(glm::vec2(element->get_position().x,
                                                    container_frame.w - element_frame.w));
                }
                    break;
                    
                case e_element_vertical_aligment_center:
                {
                    element->set_position(glm::vec2(element->get_position().x,
                                                    (container_frame.w - element_frame.w) * .5f));
                }
                    break;
            }

        }
    }
}