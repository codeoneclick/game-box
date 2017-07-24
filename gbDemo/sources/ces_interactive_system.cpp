//
//  ces_interactive_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_interactive_system.h"
#include "ces_character_controllers_component.h"
#include "ces_level_controllers_component.h"
#include "ces_level_path_grid_component.h"
#include "ces_bound_touch_component.h"

namespace game
{
    ces_interactive_system::ces_interactive_system()
    {
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_controllers_component::class_guid());
        ces_base_system::add_required_component_guid(m_level_components_mask, ces_level_path_grid_component::class_guid());
        ces_base_system::add_required_components_mask(m_level_components_mask);
        
        ces_base_system::add_required_component_guid(m_character_components_mask, ces_character_controllers_component::class_guid());
        ces_base_system::add_required_components_mask(m_character_components_mask);
    }
    
    ces_interactive_system::~ces_interactive_system()
    {
    }
    
    void ces_interactive_system::on_feed_start(f32 dt)
    {
    }
    
    void ces_interactive_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        
    }
    
    void ces_interactive_system::on_feed_end(f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_character_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            std::string character_key = entity->tag;
            auto character_component = entity->get_component<ces_character_controllers_component>();
            if(character_component->mode == ces_character_controllers_component::e_mode::e_mode_ai)
            {
                m_ai_characters[character_key] = entity;
            }
            else if(character_component->mode == ces_character_controllers_component::e_mode::e_mode_manual)
            {
                m_main_character = entity;
            }
            m_all_characters[character_key] = entity;
            ces_interactive_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_pressed);
            ces_interactive_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_released);
        });
        
        ces_base_system::enumerate_entities_with_components(m_level_components_mask, [this](const gb::ces_entity_shared_ptr& entity) {
            m_level = entity;
            ces_interactive_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_pressed);
            ces_interactive_system::add_touch_recognition(entity, gb::e_input_state::e_input_state_released);
        });
    }
    
    void ces_interactive_system::add_touch_recognition(const gb::ces_entity_shared_ptr& entity,
                                                       gb::e_input_state input_state)
    {
        const auto& bound_touch_component = entity->get_component<gb::ces_bound_touch_component>();
        if(!bound_touch_component->is_respond_to(input_state, gb::e_input_source::e_input_source_mouse_left))
        {
            bound_touch_component->enable(input_state, gb::e_input_source_mouse_left, true);
            bound_touch_component->add_callback(input_state, std::bind(&ces_interactive_system::on_touched, this,
                                                                       std::placeholders::_1,
                                                                       std::placeholders::_2,
                                                                       std::placeholders::_3,
                                                                       std::placeholders::_4));
        }
    }
    
    void ces_interactive_system::on_touched(const gb::ces_entity_shared_ptr& entity, const glm::vec2& touch_point,
                                            gb::e_input_source input_source, gb::e_input_state input_state)
    {
        
    }
}

