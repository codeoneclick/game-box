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
#include "text_label.h"
#include "ces_text_component.h"
#include "ces_material_component.h"

namespace gb
{
    namespace ui
    {
        console::console(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_lines_max_count(1),
        m_lines_current_count(0),
        m_line_height(1.f)
        {
        }
        
        console::~console()
        {
            
        }
        
        void console::create()
        {
            gb::sprite_shared_ptr console_background = control::get_fabricator()->create_sprite("console_background.xml");
            m_elements["console_background"] = console_background;
            game_object::add_child(console_background);

            control::create();
        }
        
        void console::set_size(const glm::vec2& size)
        {
            control::set_size(size);
            std::static_pointer_cast<gb::sprite>(m_elements["console_background"])->size = size;
            m_line_height = m_size.y / static_cast<f32>(m_lines_max_count);
            console::clear();
        }
        
        void console::set_lines_count(i32 lines_count)
        {
            m_lines_max_count = lines_count;
            m_line_height = m_size.y / static_cast<f32>(m_lines_max_count);
            console::clear();
        }
        
        void console::write(const std::string& message)
        {
            if(m_lines_current_count < m_lines_max_count)
            {
                std::stringstream stream;
                stream<<"console_message"<<m_lines_current_count;
                m_lines_current_count++;
                gb::text_label_shared_ptr console_message = std::static_pointer_cast<gb::text_label>(m_elements[stream.str()]);
                console_message->text = message;
            }
            else
            {
                
            }
        }
        
        void console::clear()
        {
            for(const auto& element : m_elements)
            {
                if(element.first.find("console_message") != std::string::npos)
                {
                    game_object::remove_child(element.second);
                }
            }
            
            const auto& background = m_elements["console_background"];
            m_elements.clear();
            m_elements["console_background"] = background;
            
            for(i32 i = 0; i < m_lines_max_count; ++i)
            {
                std::stringstream stream;
                stream<<"console_message"<<i;
                gb::text_label_shared_ptr console_message = control::get_fabricator()->create_text_label("console_message.xml");
                console_message->size = glm::vec2(m_size.x, m_line_height);
                console_message->text = "hello";
                console_message->position = glm::vec2(0, i * m_line_height);
                console_message->text_color = glm::vec4(0.f, 1.f, 0.f, 1.f);
                m_elements[stream.str()] = console_message;
                game_object::add_child(console_message);
                
                control::set_element_horizontal_aligment(m_elements[stream.str()], e_element_horizontal_aligment_left);
            }
            
            control::set_color("console_background", control::k_black_color);
        }
    }
}
