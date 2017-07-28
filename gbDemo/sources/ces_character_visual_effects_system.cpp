//
//  ces_character_visual_effects_system.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_character_visual_effects_system.h"
#include "ces_character_blinking_effect_component.h"
#include "ces_character_parts_component.h"
#include "sprite.h"

namespace game
{
    ces_character_visual_effects_system::ces_character_visual_effects_system()
    {
        ces_base_system::add_required_component_guid(m_blinking_effect_components_mask, ces_character_blinking_effect_component::class_guid());
        ces_base_system::add_required_components_mask(m_blinking_effect_components_mask);
    }
    
    ces_character_visual_effects_system::~ces_character_visual_effects_system()
    {
        
    }
    
    void ces_character_visual_effects_system::on_feed_start(f32 dt)
    {
        
    }
    
    void ces_character_visual_effects_system::on_feed(const gb::ces_entity_shared_ptr& root, f32 dt)
    {
        ces_base_system::enumerate_entities_with_components(m_blinking_effect_components_mask, [dt](const gb::ces_entity_shared_ptr& entity) {
            
            auto character_parts_component = entity->get_component<ces_character_parts_component>();
            auto character_blinking_effect_component = entity->get_component<ces_character_blinking_effect_component>();
            i32 blinking_count = character_blinking_effect_component->get_blinking_count();
            f32 blinking_timeinterval = character_blinking_effect_component->get_blinking_timeinterval();
            blinking_timeinterval -= dt * 1000.f;
            if(blinking_timeinterval <= 0)
            {
                if(blinking_count > 0)
                {
                    if(blinking_count % 2 == 0)
                    {
                        std::static_pointer_cast<gb::sprite>(character_parts_component->get_body_part())->color = glm::u8vec4(64);
                    }
                    else
                    {
                        std::static_pointer_cast<gb::sprite>(character_parts_component->get_body_part())->color = glm::u8vec4(255);
                    }
                    blinking_count--;
                    blinking_timeinterval = ces_character_blinking_effect_component::k_blinking_timeinterval;
                }
                else
                {
                    auto blinking_effect_ended_callback = character_blinking_effect_component->get_blinking_effect_ended_callback();
                    if(blinking_effect_ended_callback)
                    {
                        blinking_effect_ended_callback(entity);
                    }
                    std::static_pointer_cast<gb::sprite>(character_parts_component->get_body_part())->color = glm::u8vec4(0);
                    entity->remove_component(character_blinking_effect_component);
                }
            }
            character_blinking_effect_component->set_blinking_count(blinking_count);
            character_blinking_effect_component->set_blinking_timeinterval(blinking_timeinterval);
        });
    }
    
    void ces_character_visual_effects_system::on_feed_end(f32 dt)
    {
        
    }
}
