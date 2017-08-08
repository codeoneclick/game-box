//
//  action_console.cpp
//  gbUI
//
//  Created by serhii serhiiv on 3/31/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "action_console.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "label.h"
#include "ces_font_component.h"
#include "ces_material_component.h"
#include "ces_transformation_2d_component.h"
#include "ces_action_component.h"

namespace gb
{
    namespace ui
    {
        action_console::action_console(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_lines_max_count(1),
        m_line_height(1.f),
        m_visible_timeinterval(5000)
        {
            size.setter([=](const glm::vec2& size) {
                
                m_size = size;
                m_elements["console_background"]->size = size;
                m_line_height = m_size.y / static_cast<f32>(m_lines_max_count);
                action_console::clear();
                
            });
        }
        
        action_console::~action_console()
        {
            action_console::clear();
        }
        
        void action_console::create()
        {
            gb::sprite_shared_ptr console_background = control::get_fabricator()->create_sprite("console_background.xml");
            console_background->alpha = 64;
            console_background->visible = false;
            m_elements["console_background"] = console_background;
            action_console::add_child(console_background);
            
            std::shared_ptr<gb::ces_action_component> action_component = std::make_shared<gb::ces_action_component>();
            action_component->set_update_callback(std::bind(&action_console::on_message_action, this,
                                                            std::placeholders::_1, std::placeholders::_2));
            action_console::add_component(action_component);
            
            control::create();
        }
        
        void action_console::set_lines_count(i32 lines_count)
        {
            m_lines_max_count = lines_count;
            m_line_height = m_size.y / static_cast<f32>(m_lines_max_count);
            action_console::clear();
        }
        
        void action_console::write(const std::string& message)
        {
            if(m_unused_messages.size() > 0)
            {
                gb::label_shared_ptr console_message =  std::static_pointer_cast<gb::label>(m_unused_messages.front().lock());
                m_unused_messages.pop_front();
                console_message->text = message;
                console_message->font_color = glm::u8vec4(0, 255, 0, 255);
                m_messages.push_back(std::make_tuple(console_message, m_visible_timeinterval));
                action_console::update_lines_position();
            }
            else
            {
                auto it = m_messages.front();
                gb::label_shared_ptr console_message = std::static_pointer_cast<gb::label>(std::get<0>(it).lock());
                console_message->text = message;
                m_messages.pop_front();
                m_messages.push_back(std::make_tuple(console_message, m_visible_timeinterval));
                action_console::update_lines_position();
            }
        }
        
        void action_console::clear()
        {
            for(const auto& element : m_elements)
            {
                if(element.first.find("console_message") != std::string::npos)
                {
                    action_console::remove_child(element.second);
                }
            }
            m_messages.clear();
            m_unused_messages.clear();
            
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
                console_message->font_size = m_line_height;
                auto transformation_component = console_message->get_component<ces_transformation_2d_component>();
                transformation_component->set_is_in_camera_space(false);
                
                m_elements[stream.str()] = console_message;
                action_console::add_child(console_message);
                m_unused_messages.push_back(console_message);
                
                control::set_element_horizontal_aligment(m_elements[stream.str()], e_element_horizontal_aligment_left);
            }
            control::set_color("console_background", control::k_black_color);
        }
        
        void action_console::on_message_action(const gb::ces_entity_shared_ptr& entity, f32 dt)
        {
            for(auto& it : m_messages)
            {
                gb::label_shared_ptr console_message = std::static_pointer_cast<gb::label>(std::get<0>(it).lock());
                std::get<1>(it) -= dt * 1000.f;
                std::get<1>(it) = std::max(std::get<1>(it), 0.f);
                console_message->font_color = glm::mix(glm::u8vec4(0, 255, 0, 0),  glm::u8vec4(0, 255, 0, 255), std::get<1>(it) / m_visible_timeinterval);
                if(std::get<1>(it) == 0.f && console_message == std::get<0>(m_messages.front()).lock())
                {
                    m_unused_messages.push_back(console_message);
                    m_messages.pop_front();
                    action_console::update_lines_position();
                    break;
                }
            }
        }
        
        void action_console::update_lines_position()
        {
            i32 index = 0;
            for(const auto& message : m_messages)
            {
                gb::label_shared_ptr console_message = std::static_pointer_cast<gb::label>(std::get<0>(message).lock());
                console_message->position = glm::vec2(0, index * m_line_height);
                index++;
            }
        }
    }
}
