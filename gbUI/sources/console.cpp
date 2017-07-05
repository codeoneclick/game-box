//
//  console.cpp
//  gbUI
//
//  Created by serhii serhiiv on 12/15/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "console.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "label.h"
#include "ces_font_component.h"
#include "ces_material_component.h"
#include "ces_transformation_2d_component.h"

namespace gb
{
    namespace ui
    {
        console::console(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_lines_max_count(1),
        m_line_height(1.f)
        {
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                m_elements["console_background"]->size = size;
                m_line_height = m_size.y / static_cast<f32>(m_lines_max_count);
                console::clear();
                
            });
        }
        
        console::~console()
        {
            console::clear();
        }
        
        console_shared_ptr console::construct(const scene_fabricator_shared_ptr& fabricator)
        {
            auto entity = std::make_shared<console>(fabricator);
            return entity;
        }
        
        void console::create()
        {
            gb::sprite_shared_ptr console_background = control::get_fabricator()->create_sprite("console_background.xml");
            m_elements["console_background"] = console_background;
            console::add_child(console_background);

            control::create();
        }
        
        void console::set_lines_count(i32 lines_count)
        {
            m_lines_max_count = lines_count;
            m_line_height = m_size.y / static_cast<f32>(m_lines_max_count);
            console::clear();
        }
        
        void console::write(const std::string& message)
        {
            if(m_messages.size() < m_lines_max_count)
            {
                std::stringstream stream;
                stream<<"console_message"<<m_messages.size();
                gb::label_shared_ptr console_message = std::static_pointer_cast<gb::label>(m_elements[stream.str()]);
                console_message->text = message;
                m_messages.push_back(console_message);
            }
            else
            {
                gb::label_shared_ptr console_message = std::static_pointer_cast<gb::label>(m_messages.front().lock());
                console_message->text = message;
                m_messages.pop_front();
                m_messages.push_back(console_message);
                i32 index = 0;
                for(const auto& message : m_messages)
                {
                    gb::label_shared_ptr console_message = std::static_pointer_cast<gb::label>(message.lock());
                    console_message->position = glm::vec2(0, index * m_line_height);
                    index++;
                }
            }
        }
        
        void console::clear()
        {
            for(const auto& element : m_elements)
            {
                if(element.first.find("console_message") != std::string::npos)
                {
                    console::remove_child(element.second);
                }
            }
            m_messages.clear();
            
            auto background = m_elements["console_background"];
            m_elements.clear();
            m_elements["console_background"] = background;
            
            for(i32 i = 0; i < m_lines_max_count; ++i)
            {
                std::stringstream stream;
                stream<<"console_message"<<i;
                gb::label_shared_ptr console_message = control::get_fabricator()->create_label("console_message.xml");
                console_message->size = glm::vec2(m_size.x, m_line_height);
                console_message->text = "";
                console_message->position = glm::vec2(0, i * m_line_height);
                console_message->font_color = glm::u8vec4(0, 255, 0, 255);
                auto transformation_component = console_message->get_component<ces_transformation_2d_component>();
                transformation_component->set_is_in_camera_space(false);
                m_elements[stream.str()] = console_message;
                console::add_child(console_message);
                
                control::set_element_horizontal_aligment(m_elements[stream.str()], e_element_horizontal_aligment_left);
            }
            
            control::set_color("console_background", control::k_black_color);
        }
    }
}
