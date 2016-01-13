//
//  button.cpp
//  gbUI
//
//  Created by sergey.sergeev on 1/13/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "button.h"
#include "scene_fabricator.h"
#include "sprite.h"
#include "label.h"
#include "ces_geometry_quad_component.h"

namespace gb
{
    namespace ui
    {
        button::button(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator)
        {
            ces_geometry_component_shared_ptr geometry_component = std::make_shared<ces_geometry_quad_component>();
            ces_entity::add_component(geometry_component);
        }
        
        button::~button()
        {
            
        }
        
        void button::create()
        {
            gb::sprite_shared_ptr button_background = control::get_fabricator()->create_sprite("button_background.xml");
            m_elements["button_background"] = button_background;
            ces_entity::add_child(button_background);
            
            gb::label_shared_ptr button_label = control::get_fabricator()->create_label("button_label.xml");
            m_elements["button_label"] = button_label;
            ces_entity::add_child(button_label);
        }
        
        void button::set_size(const glm::vec2& size)
        {
            unsafe_get_geometry_quad_component_from_this->set_size(size);
            std::static_pointer_cast<gb::sprite>(m_elements["button_background"])->set_size(size);
        }
        
        glm::vec2 button::get_size() const
        {
            return unsafe_get_geometry_quad_component_from_this->get_size();
        }
        
        void button::set_text(const std::string& text)
        {
            std::static_pointer_cast<gb::label>(m_elements["button_label"])->set_text(text);
        }
        
        std::string button::get_text()
        {
            return std::static_pointer_cast<gb::label>(m_elements["button_label"])->get_text();
        }
    }
}