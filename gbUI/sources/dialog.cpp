//
//  dialog.cpp
//  gbUI
//
//  Created by sergey.sergeev on 9/28/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "dialog.h"
#include "control.h"
#include "sprite.h"
#include "scene_fabricator.h"
#include "label.h"

namespace gb
{
    namespace ui
    {
        dialog::dialog(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::interaction_control(fabricator)
        {
            
        }
        
        dialog::~dialog()
        {
            
        }
        
        void dialog::create()
        {
            gb::sprite_shared_ptr background = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_background_element_name] = background;
            dialog::add_child(background);
            
            gb::sprite_shared_ptr title_background = control::get_fabricator()->create_sprite("button_background.xml");
            title_background->color = glm::u8vec4(16, 16, 16, 255);
            m_elements["title_background"] = title_background;
            dialog::add_child(title_background);
            
            gb::label_shared_ptr label = control::get_fabricator()->create_label_2d("button_label.xml");
            label->font_size = 32.f;
            label->text = "LABEL";
            label->font_color = k_white_color;
            m_elements[k_label_element_name] = label;
            dialog::add_child(label);
            m_label_color = k_white_color;
            
            interaction_control::create();
        }
        
        const std::map<i32, ces_entity_shared_ptr>& dialog::get_controls() const
        {
            return m_controls;
        }
        
        void dialog::add_control(const ces_entity_shared_ptr& control, i32 id)
        {
            const auto& iterator = m_controls.find(id);
            if(iterator == m_controls.end())
            {
                m_controls.insert(std::make_pair(id, control));
                dialog::add_child(control);

                const auto game_object_2d = std::static_pointer_cast<gb::game_object_2d>(control);
                glm::vec2 game_object_2d_position = game_object_2d->position;
                game_object_2d->enumerate_children([=](const ces_entity_shared_ptr& enumerator) {
                    const auto child = std::static_pointer_cast<gb::game_object_2d>(enumerator);
                    glm::vec2 child_position = child->position;
                    glm::vec2 result_position = game_object_2d_position + child_position;
                    
                    glm::vec2 child_size = child->size;
                    glm::vec2 result_size = game_object_2d_position + child_position + child_size;
                    
                    if (m_bounds.x > result_position.x)
                    {
                        m_bounds.x = result_position.x;
                    }
                    if (m_bounds.y > result_position.y)
                    {
                        m_bounds.y = result_position.y;
                    }
                    
                    if (m_bounds.z < result_size.x)
                    {
                        m_bounds.z = result_size.x;
                    }
                    if (m_bounds.w < result_size.y)
                    {
                        m_bounds.w = result_size.y;
                    }
                });
                get_element_as<gb::game_object_2d>(k_background_element_name)->position = glm::vec2(m_bounds.x - 16.f, m_bounds.y - 16.f);
                get_element_as<gb::game_object_2d>(k_background_element_name)->size = glm::vec2(m_bounds.z - m_bounds.x + 32.f, m_bounds.w - m_bounds.y + 32.f);
                
                get_element_as<gb::game_object_2d>("title_background")->position = glm::vec2(m_bounds.x - 16.f, m_bounds.y - 16.f - 48.f);
                get_element_as<gb::game_object_2d>("title_background")->size = glm::vec2(m_bounds.z - m_bounds.x + 32.f, 48.f);
                
                size = glm::vec2(m_bounds.z - m_bounds.x + 32.f, m_bounds.w - m_bounds.y + 32.f);
                std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->position = glm::vec2(0.f, m_bounds.y - 56.f);
                std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->size = glm::vec2(m_bounds.z - m_bounds.x + 32.f, 32.f);
                control::set_element_horizontal_aligment(m_elements[k_label_element_name], e_element_horizontal_aligment_left);
            }
            else
            {
                assert(false);
            }
        }
        
        void dialog::remove_control(i32 id)
        {
            const auto& iterator = m_controls.find(id);
            if(iterator != m_controls.end())
            {
                m_controls.erase(iterator);
                dialog::remove_child(iterator->second);
            }
            else
            {
                assert(false);
            }
        }
        
        ces_entity_shared_ptr dialog::get_control(i32 id) const
        {
            const auto& iterator = m_controls.find(id);
            if(iterator != m_controls.end())
            {
                return iterator->second;
            }
            else
            {
                assert(false);
                return nullptr;
            }
        }
        
        void dialog::show_title(bool value)
        {
            std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->font_color = glm::u8vec4(value ? 255 : 0);
            get_element_as<gb::sprite>("title_background")->color = glm::u8vec4(value ? 255 : 0);
        }
        
        void dialog::set_title(const std::string& title)
        {
            std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->text = title;
        }
        
        void dialog::set_title_color(const glm::u8vec4& color)
        {
            get_element_as<gb::sprite>("title_background")->color = color;
        }
        
        void dialog::set_title_text_color(const glm::u8vec4& color)
        {
            std::static_pointer_cast<gb::label>(m_elements[k_label_element_name])->font_color = color;
        }
    }
}
