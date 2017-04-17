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
#include "ces_geometry_component.h"
#include "ces_font_component.h"
#include "mesh.h"
#include "vbo.h"
#include "glm_extensions.h"
#include "scene_fabricator.h"

namespace gb
{
    namespace ui
    {
        glm::u8vec4 control::k_black_color = glm::u8vec4(0, 0, 0, 255);
        glm::u8vec4 control::k_dark_gray_color = glm::u8vec4(64, 64, 64, 255);
        glm::u8vec4 control::k_gray_color = glm::u8vec4(128, 128, 128, 255);
        glm::u8vec4 control::k_light_gray_color = glm::u8vec4(192, 192, 192, 255);
        glm::u8vec4 control::k_white_color = glm::u8vec4(255, 255, 255, 255);
        glm::u8vec4 control::k_green_color = glm::u8vec4(0, 255, 0, 255);
        
        std::string control::k_background_element_name = "ui_background";
        std::string control::k_label_element_name = "ui_label";

        control::control(const scene_fabricator_shared_ptr& fabricator) :
        m_fabricator(fabricator)
        {
            auto transformation_component = ces_entity::get_unsafe_component<ces_transformation_component>();
            transformation_component->set_is_in_camera_space(false);
            
            size.setter([=](const glm::vec2& size) {
                m_size = size;
            });
            size.getter([=]() {
                return m_size;
            });
        }
        
        control::~control()
        {
            m_elements.clear();
        }
        
        scene_fabricator_shared_ptr control::get_fabricator() const
        {
            return m_fabricator.lock();
        }
        
        void control::set_element_horizontal_aligment(const game_object_shared_ptr& element,
                                                      e_element_horizontal_aligment aligment)
        {
            glm::vec2 container_size = control::size;
            glm::vec2 element_size = element->size;
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
                    element->position = glm::vec2(container_size.x - element_size.x,
                                                  element_position.y);
                }
                    break;
                    
                case e_element_horizontal_aligment_center:
                {
                    element->position = glm::vec2((container_size.x - element_size.x) * .5f,
                                                  element_position.y);
                }
                    break;
            }
        }
        
        void control::set_element_vertical_aligment(const game_object_shared_ptr& element,
                                                    e_element_vertical_aligment aligment)
        {
            glm::vec2 container_size = control::size;
            glm::vec2 element_size = element->size;
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
                                                  container_size.y - element_size.y);
                }
                    break;
                    
                case e_element_vertical_aligment_center:
                {
                    element->position = glm::vec2(element_position.x,
                                                  (container_size.y - element_size.y) * .5f);
                }
                    break;
            }
        }
        
        void control::create()
        {
            for(const auto& element : m_elements)
            {
                auto transformation_component = element.second->get_unsafe_component<ces_transformation_component>();
                transformation_component->set_is_in_camera_space(false);
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
        
        void control::set_color(const std::string& element_name, const glm::u8vec4& color)
        {
            const auto& element = m_elements[element_name];
            if(element)
            {
                const auto& font_component = element->get_component<ces_font_component>();
                if(font_component)
                {
                    font_component->set_font_color(color);
                    font_component->generate_geometry();
                }
                else
                {
                    const auto& geometry_component = element->get_component<ces_geometry_component>();
                    if(geometry_component)
                    {
                        const auto& mesh = geometry_component->get_mesh();
                        if(mesh)
                        {
                            vbo::vertex_attribute* vertices = mesh->get_vbo()->lock();
                            i32 vertices_count = mesh->get_vbo()->get_used_size();
                            for(i32 i = 0; i < vertices_count; ++i)
                            {
                                vertices[i].m_color = color;
                            }
                            mesh->get_vbo()->unlock();
                        }
                    }
                }
            }
        }
    }
}
