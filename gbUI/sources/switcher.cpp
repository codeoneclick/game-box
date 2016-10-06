//
//  switcher.cpp
//  gbUI
//
//  Created by Serhii Serhiiv on 2/6/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "switcher.h"
#include "scene_fabricator.h"
#include "ces_bound_touch_component.h"
#include "ces_transformation_component.h"
#include "ces_material_component.h"
#include "ces_action_component.h"
#include "sprite.h"
#include "button.h"

namespace gb
{
    namespace ui
    {
        static const std::string k_color_state_uniform = "u_color";
        static const std::string k_on_state_label = "on";
        static const std::string k_off_state_label = "off";
        
        switcher::switcher(const scene_fabricator_shared_ptr& fabricator) :
        gb::ui::control(fabricator),
        m_on_switch_callback(nullptr),
        m_current_value(false)
        {
            
        }
        
        switcher::~switcher()
        {
            
        }
        
        void switcher::create()
        {
            gb::sprite_shared_ptr switcher_background =
            control::get_fabricator()->create_sprite("switcher_background.xml");
            m_elements["switcher_background"] = switcher_background;
            game_object::add_child(switcher_background);
            
            ces_material_component* material_component = unsafe_get_material_component(switcher_background);
            material_component->set_custom_shader_uniform(control::k_dark_gray_color, k_color_state_uniform);
            
            m_button = std::make_shared<gb::ui::button>(control::get_fabricator());
            m_button->create();
            m_button->set_size(glm::vec2(32.f));
            m_button->position = glm::vec2(0.f);
            m_button->set_text(k_off_state_label);
            m_button->set_on_pressed_callback(std::bind(&switcher::on_switch, this, std::placeholders::_1));
            game_object::add_child(m_button);
            
            control::create();
        }
        
        void switcher::set_size(const glm::vec2& size)
        {
            control::set_size(size);
            std::static_pointer_cast<gb::sprite>(m_elements["switcher_background"])->size = size;
            m_button->set_size(glm::vec2(size.x * .5f, size.y));
        }
        
        void switcher::set_on_switch_callback(const t_on_switch_callback& callback)
        {
            m_on_switch_callback = callback;
        }
        
        void switcher::switch_value(bool value)
        {
            if(value != m_current_value)
            {
                gb::ces_action_component_shared_ptr action_component = std::make_shared<gb::ces_action_component>();
                action_component->set_update_callback(std::bind(&switcher::on_switching, this, std::placeholders::_1, std::placeholders::_2));
                switcher::add_component(action_component);
                m_button->set_on_pressed_callback(nullptr);
            }
            m_current_value = value;
        }
        
        void switcher::on_switching(const ces_entity_shared_ptr &entity, f32 deltatime)
        {
            glm::vec2 button_position = m_button->position;
            if(m_current_value && button_position.x < m_size.x - m_button->get_size().x)
            {
                button_position.x += 150.f * deltatime;
                button_position.x = std::min(button_position.x, m_size.x - m_button->get_size().x);
                m_button->position = button_position;
                return;
            }
            else if(!m_current_value && button_position.x > 0.f)
            {
                button_position.x -= 150.f * deltatime;
                button_position.x = std::max(button_position.x, 0.f);
                m_button->position = button_position;
                return;
            }
            
            switcher::remove_component(gb::ces_action_component::class_guid());
            m_button->set_on_pressed_callback(std::bind(&switcher::on_switch, this, std::placeholders::_1));
            m_button->set_text(m_current_value ? k_on_state_label : k_off_state_label);
            
            if(m_on_switch_callback)
            {
                m_on_switch_callback(m_current_value, shared_from_this());
            }
        }
        
        void switcher::on_switch(const ces_entity_shared_ptr& entity)
        {
            switcher::switch_value(!m_current_value);
        }
    }
}
