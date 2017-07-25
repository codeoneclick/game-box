//
//  ces_state_automat_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/25/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_state_automat_system.h"
#include "ces_character_state_automat_component.h"
#include "ai_actions_processor.h"

namespace game
{
    ces_state_automat_system::ces_state_automat_system()
    {
        ces_base_system::add_required_component_guid(m_state_automat_components_mask, ces_character_state_automat_component::class_guid());
        ces_base_system::add_required_components_mask(m_state_automat_components_mask);
    }
    
    ces_state_automat_system::~ces_state_automat_system()
    {
        
    }
    
    void ces_state_automat_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_state_automat_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_state_automat_components_mask, [dt](const gb::ces_entity_shared_ptr& entity) {
            
            std::string character_key = entity->tag;
            auto character_state_automat_component = entity->get_component<ces_character_state_automat_component>();
            auto actions_processor = character_state_automat_component->get_actions_processor();
            actions_processor->update(dt);
        });
    }
    
    void ces_state_automat_system::on_feed_end(f32 dt)
    {
        
    }
}
