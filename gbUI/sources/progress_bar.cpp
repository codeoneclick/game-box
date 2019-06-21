//
//  progress_bar.cpp
//  gbUI
//
//  Created by serhii.s on 6/12/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "progress_bar.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "ces_bound_touch_2d_component.h"
#include "ces_material_component.h"
#include "ces_action_component.h"
#include "ces_transformation_extension.h"
#include "game_command.h"
#include "input_context.h"
#include "glm_extensions.h"

namespace gb
{
    namespace ui
    {
        progress_bar::progress_bar(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::interaction_control(fabricator)
        {
            size.setter([=](const glm::vec2& size) {
                m_size = size;
                
                std::static_pointer_cast<gb::sprite>(m_elements[k_background_element_name])->size = size;
                std::static_pointer_cast<gb::sprite>(m_elements[k_foreground_element_name])->size = glm::vec2(size.x - 4.f, size.y - 4.f);
                std::static_pointer_cast<gb::sprite>(m_elements[k_progress_line_element_name])->size = glm::vec2(size.x - 8.f, size.y - 8.f);
                
                control::set_element_horizontal_aligment(m_elements[k_progress_line_element_name], e_element_horizontal_aligment_left);
                control::set_element_vertical_aligment(m_elements[k_progress_line_element_name], e_element_vertical_aligment_center);
                
                control::set_element_horizontal_aligment(m_elements[k_foreground_element_name], e_element_horizontal_aligment_center);
                control::set_element_vertical_aligment(m_elements[k_foreground_element_name], e_element_vertical_aligment_center);
            });
            
            
        }
        
        progress_bar::~progress_bar()
        {
            
        }
        
        void progress_bar::setup_components()
        {
            interaction_control::setup_components();
        }
        
        void progress_bar::create()
        {
            gb::sprite_shared_ptr background_element = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_background_element_name] = background_element;
            add_child(background_element);
            
            gb::sprite_shared_ptr foreground_element = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_foreground_element_name] = foreground_element;
            add_child(foreground_element);
            
            gb::sprite_shared_ptr progress_line_element = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements[k_progress_line_element_name] = progress_line_element;
            add_child(progress_line_element);
            
            interaction_control::create();
            
            control::set_color(k_background_element_name, control::k_light_gray_color);
            control::set_color(k_foreground_element_name, control::k_dark_gray_color);
            control::set_color(k_progress_line_element_name, control::k_white_color);
            
            control::set_element_horizontal_aligment(m_elements[k_progress_line_element_name], e_element_horizontal_aligment_left);
            control::set_element_vertical_aligment(m_elements[k_progress_line_element_name], e_element_vertical_aligment_center);
            
            control::set_element_horizontal_aligment(m_elements[k_foreground_element_name], e_element_horizontal_aligment_center);
            control::set_element_vertical_aligment(m_elements[k_foreground_element_name], e_element_vertical_aligment_center);
        }
        
        void progress_bar::set_progress(f32 progress)
        {
            m_progress = progress;
            m_progress = glm::clamp(m_progress, 0.f, 1.f);
            glm::vec2 current_size = size;
            current_size.x -= 8.f;
            current_size.y -= 8.f;
            current_size.x *= m_progress;
            std::static_pointer_cast<gb::sprite>(m_elements[k_progress_line_element_name])->size = current_size;
            
            control::set_element_horizontal_aligment(m_elements[k_progress_line_element_name], e_element_horizontal_aligment_left);
            
            glm::vec2 current_position = m_elements[k_progress_line_element_name]->position;
            current_position.x += 4;
            std::static_pointer_cast<gb::sprite>(m_elements[k_progress_line_element_name])->position = current_position;
        }
        
        f32 progress_bar::get_progress() const
        {
            return m_progress;
        }
        
        void progress_bar::set_progress_line_color(const glm::u8vec4& color)
        {
            control::set_color(k_progress_line_element_name, color);
        }
        
        void progress_bar::set_foreground_color(const glm::u8vec4& color)
        {
            control::set_color(k_foreground_element_name, color);
        }
    }
}
