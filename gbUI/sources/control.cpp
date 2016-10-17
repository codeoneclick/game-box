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
        glm::vec4 control::k_green_color = glm::vec4(0.f, 1.f, 0.f, 1.f);

        
        control::control(const scene_fabricator_shared_ptr& fabricator) :
        m_fabricator(fabricator),
        m_visible(true)
        {
            unsafe_get_transformation_component_from_this->set_is_in_camera_space(false);
            
            bound.getter([=]() {
                glm::vec4 bound = glm::vec4(0.f);
                
                ces_transformation_component* transformation_component = unsafe_get_transformation_component_from_this;
                glm::vec2 min_bound = glm::transform(glm::vec2(0.f),
                                                     transformation_component->get_matrix_m()) - transformation_component->get_position();
                glm::vec2 max_bound = glm::transform(m_size,
                                                     transformation_component->get_matrix_m()) - transformation_component->get_position();
                bound = glm::vec4(min_bound, max_bound);
                return bound;
            });
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
            glm::vec4 container_bound = control::bound;
            glm::vec4 element_bound = element->bound;
            glm::vec2 element_position = element->position;
            
            switch (aligment)
            {
                case e_element_horizontal_aligment_left:
                {
                    element->position = glm::vec2(0.f, element_position.y);
                }
                    break;
                    
                case e_element_horizontal_aligment_right:
                {
                    element->position = glm::vec2(container_bound.z - element_bound.z,
                                                  element_position.y);
                }
                    break;
                    
                case e_element_horizontal_aligment_center:
                {
                    element->position = glm::vec2((container_bound.z - element_bound.z) * .5f,
                                                  element_position.y);
                }
                    break;
            }
        }
        
        void control::set_element_vertical_aligment(const game_object_shared_ptr& element,
                                                    e_element_vertical_aligment aligment)
        {
            glm::vec4 container_bound = control::bound;
            glm::vec4 element_bound = element->bound;
            glm::vec2 element_position = element->position;
            
            switch (aligment)
            {
                case e_element_vertical_aligment_top:
                {
                    element->position = glm::vec2(element_position.x, 0.f);
                }
                    break;
                    
                case e_element_vertical_aligment_down:
                {
                    element->position = glm::vec2(element_position.x,
                                                  container_bound.w - element_bound.w);
                }
                    break;
                    
                case e_element_vertical_aligment_center:
                {
                    element->position = glm::vec2(element_position.x,
                                                  (container_bound.w - element_bound.w) * .5f);
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
        
        void control::create()
        {
            for(const auto& element : m_elements)
            {
                unsafe_get_transformation_component(element.second)->set_is_in_camera_space(false);
            }
            
            ces_material_component_shared_ptr material_component = ces_entity::get_component<ces_material_component>();
            if(material_component)
            {
                material_component->set_is_batching(true);
            }
            for(const auto& element : m_elements)
            {
                material_component = element.second->get_component<ces_material_component>();
                if(material_component)
                {
                    material_component->set_is_batching(true);
                }
            }
        }
    }
}
