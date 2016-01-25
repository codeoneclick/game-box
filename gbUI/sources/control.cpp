//
//  control.cpp
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "control.h"
#include "ces_transformation_component.h"
#include "ces_material_component.h"
#include "glm_extensions.h"

namespace gb
{
    namespace ui
    {
        glm::vec4 control::k_black_color = glm::vec4(0.f, 0.f, 0.f, 1.f);
        glm::vec4 control::k_dark_gray_color = glm::vec4(.15f, .15f, .15f, 1.f);
        glm::vec4 control::k_gray_color = glm::vec4(.3f, .3f, .3f, 1.f);
        glm::vec4 control::k_light_gray_color = glm::vec4(.45f, .45f, .45f, 1.f);
        glm::vec4 control::k_white_color = glm::vec4(1.f, 1.f, 1.f, 1.f);

        
        control::control(const scene_fabricator_shared_ptr& fabricator) :
        m_fabricator(fabricator),
        m_visible(true)
        {
            unsafe_get_transformation_component_from_this->set_is_in_camera_space(false);
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
        
        void control::set_size(const glm::vec2& size)
        {
            m_size = size;
        }
        
        glm::vec2 control::get_size() const
        {
            return m_size;
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
        
        void control::create()
        {
            for(const auto& element : m_elements)
            {
                unsafe_get_transformation_component(element.second)->set_is_in_camera_space(false);
            }
        }
        
        void control::set_visible(bool value)
        {
            m_visible = value;
            for(const auto& element : m_elements)
            {
                ces_material_component* material_component = unsafe_get_material_component(element.second);
                if(material_component)
                {
                    material_component->set_visible(m_visible);
                }
            }
        }
        
        bool control::get_visible() const
        {
            return m_visible;
        }
    }
}