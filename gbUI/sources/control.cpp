//
//  control.cpp
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "control.h"
#include "ces_transformation_component.h"
#include "glm_extensions.h"

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
            glm::vec4 container_frame = control::get_bound();
            glm::vec4 element_frame = element->get_bound();
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
            glm::vec4 container_frame = control::get_bound();
            glm::vec4 element_frame = element->get_bound();
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
        
        glm::vec4 control::get_bound() const
        {
            glm::vec4 bound = glm::vec4(0.f);
            
            ces_transformation_component* transformation_component = unsafe_get_transformation_component_from_this;
            glm::vec2 min_bound = glm::transform(glm::vec2(0.f),
                                                 transformation_component->get_matrix_m()) - transformation_component->get_position();
            glm::vec2 max_bound = glm::transform(m_size,
                                                 transformation_component->get_matrix_m()) - transformation_component->get_position();
            bound = glm::vec4(min_bound.x, min_bound.y, max_bound.x, max_bound.y);
            return bound;
        }
    }
}