//
//  control.cpp
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "control.h"
#include "ces_transformation_2d_component.h"
#include "ces_material_component.h"
#include "ces_geometry_component.h"
#include "ces_font_component.h"
#include "mesh_2d.h"
#include "vbo.h"
#include "glm_extensions.h"
#include "scene_fabricator.h"
#include "ces_sound_component.h"
#include "ces_ui_control_component.h"
#include "ces_action_component.h"

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
            ces_entity::add_deferred_component_constructor<ces_ui_control_component>();
            
            m_background_color.fill(k_gray_color);
            
            size.setter([=](const glm::vec2& size) {
                m_size = size;
            });
            size.getter([=]() {
                auto transformation_component = get_component<ces_transformation_component>();
                return m_size * transformation_component->as_2d()->get_scale();
            });
        }
        
        control::~control()
        {
            m_elements.clear();
        }
        
        void control::setup_components()
        {
            auto transformation_component = ces_entity::get_component<ces_transformation_2d_component>();
            transformation_component->set_is_in_camera_space(false);
        }
        
        scene_fabricator_shared_ptr control::get_fabricator() const
        {
            return m_fabricator.lock();
        }
        
        void control::set_element_horizontal_aligment(const game_object_2d_shared_ptr& element,
                                                      e_element_horizontal_aligment aligment,
													  const glm::vec2& custom_container_size)
        {
			glm::vec2 container_size = custom_container_size.x > 0.f && custom_container_size.y > 0.f ? custom_container_size : control::size;
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
        
        void control::set_element_vertical_aligment(const game_object_2d_shared_ptr& element,
                                                    e_element_vertical_aligment aligment,
													const glm::vec2& custom_container_size)
        {
			glm::vec2 container_size = custom_container_size.x > 0.f && custom_container_size.y > 0.f ? custom_container_size : control::size;
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
                auto transformation_component = element.second->get_component<ces_transformation_2d_component>();
                transformation_component->set_is_in_camera_space(false);
            }
        }
        
        void control::set_color(const std::string& element_name, const glm::u8vec4& color)
        {
            const auto& element = m_elements[element_name];
            if (element)
            {
                const auto& font_component = element->get_component<ces_font_component>();
                if (font_component)
                {
                    font_component->set_font_color(color);
                    font_component->request_mesh_2d();
                }
                else
                {
                    const auto& geometry_component = element->get_component<ces_geometry_component>();
                    if (geometry_component)
                    {
                        const auto& mesh = geometry_component->get_mesh();
                        if (mesh)
                        {
                            vbo::vertex_attribute_PTNTC* vertices = mesh->get_vbo()->lock<vbo::vertex_attribute_PTNTC>();
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
        
        void control::attach_sound(const std::string& filename, const std::string& state)
        {
            auto sound_component = ces_entity::get_component<gb::al::ces_sound_component>();
            if(!sound_component)
            {
                sound_component = std::make_shared<gb::al::ces_sound_component>();
                ces_entity::add_component(sound_component);
            }
            sound_component->add_sound(filename, false);
            m_sounds_linkage.insert(std::make_pair(state, filename));
        }
        
        void control::set_background_color(const glm::u8vec4& color, e_control_state state)
        {
            control::set_color(control::k_background_element_name, color);
            m_background_color[static_cast<i32>(state)] = color;
        }
        
        void control::set_alpha(ui8 alpha)
        {
            for(const auto& element : m_elements)
            {
                const auto ui_control_component = element.second->get_component<ces_ui_control_component>();
                if (ui_control_component)
                {
                    element.second->as<control>()->set_alpha(alpha);
                }
                const auto& font_component = element.second->get_component<ces_font_component>();
                if (font_component)
                {
                    glm::u8vec4 current_color = font_component->get_font_color();
                    current_color.a = alpha;
                    font_component->set_font_color(current_color);
                    font_component->request_mesh_2d();
                }
                else
                {
                    const auto& geometry_component = element.second->get_component<ces_geometry_component>();
                    if (geometry_component)
                    {
                        const auto& mesh = geometry_component->get_mesh();
                        if (mesh)
                        {
                            vbo::vertex_attribute_PTNTC* vertices = mesh->get_vbo()->lock<vbo::vertex_attribute_PTNTC>();
                            i32 vertices_count = mesh->get_vbo()->get_used_size();
                            glm::u8vec4 current_color = vertices[0].m_color;
                            current_color.a = alpha;
                            for(i32 i = 0; i < vertices_count; ++i)
                            {
                                vertices[i].m_color = current_color;
                            }
                            mesh->get_vbo()->unlock();
                        }
                    }
                }
            }
        }
        
        void control::focus(bool value, f32 focus_interval_in_seconds, const on_focus_callback_t& callback)
        {
            if (value)
            {
                m_on_focus_callback = callback;
                m_focus_interval_in_seconds = focus_interval_in_seconds;
                auto action_component = get_component<ces_action_component>();
                if (!action_component)
                {
                    action_component = std::make_shared<ces_action_component>();
                    add_component(action_component);
                }
                action_component->set_update_callback([=](const ces_entity_shared_ptr& entity, f32 dt) {
                    m_current_focus_interval += (m_focus_state == e_focus_state::e_inc) ? dt : -dt;
                    m_current_focus_interval = glm::clamp(m_current_focus_interval, 0.f, m_focus_interval_in_seconds);
                    if (m_current_focus_interval >= m_focus_interval_in_seconds || m_current_focus_interval <= 0.f)
                    {
                        m_focus_state = m_focus_state == e_focus_state::e_inc ? e_focus_state::e_dec : e_focus_state::e_inc;
                    }
                    f32 delta = glm::clamp(m_current_focus_interval / m_focus_interval_in_seconds, 0.f, 1.f);
                    control::set_color(control::k_background_element_name, glm::mix(m_background_color[static_cast<i32>(e_control_state::e_none)],
                                                                                    m_background_color[static_cast<i32>(e_control_state::e_focused)],
                                                                                    delta));
                    
                    if (m_on_focus_callback)
                    {
                        m_on_focus_callback(dt);
                    }
                });
            }
            else
            {
                remove_component(ces_action_component::class_guid());
            }
        }
        
        void control::disable(bool value)
        {
            if (value)
            {
                control::set_color(control::k_background_element_name, m_background_color[static_cast<i32>(e_control_state::e_disabled)]);
            }
            else
            {
                control::set_color(control::k_background_element_name, m_background_color[static_cast<i32>(e_control_state::e_none)]);
            }
        }
    }
}
