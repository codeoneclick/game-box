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
        glm::vec4 control::k_black_color = glm::vec4(0.f, 0.f, 0.f, 1.f);
        glm::vec4 control::k_dark_gray_color = glm::vec4(.15f, .15f, .15f, 1.f);
        glm::vec4 control::k_gray_color = glm::vec4(.3f, .3f, .3f, 1.f);
        glm::vec4 control::k_light_gray_color = glm::vec4(.45f, .45f, .45f, 1.f);
        glm::vec4 control::k_white_color = glm::vec4(1.f, 1.f, 1.f, 1.f);
        glm::vec4 control::k_green_color = glm::vec4(0.f, 1.f, 0.f, 1.f);

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
        
        void control::set_color(const std::string& element_name, const glm::vec4& color)
        {
            const auto& element = m_elements[element_name];
            if(element)
            {
                glm::u8vec4 color_u8 = glm::u8vec4(static_cast<ui8>(color.r * 255.f),
                                                   static_cast<ui8>(color.g * 255.f),
                                                   static_cast<ui8>(color.b * 255.f),
                                                   static_cast<ui8>(color.a * 255.f));
                const auto& font_component = element->get_component<ces_font_component>();
                if(font_component)
                {
                    font_component->set_font_color(color_u8);
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
                                vertices[i].m_color = color_u8;
                            }
                            mesh->get_vbo()->unlock();
                        }
                    }
                }
            }
        }
    }
}
