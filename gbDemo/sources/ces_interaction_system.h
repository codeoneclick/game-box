//
//  ces_interaction_system.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/24/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"
#include "input_context.h"

namespace game
{
    class ces_interaction_system : public gb::ces_base_system
    {
    private:
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_level_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_character_components_mask;
        gb::ces_entity_weak_ptr m_level;
        gb::ces_entity_weak_ptr m_main_character;
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_ai_characters;
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_all_characters;
        
        void on_touched(const gb::ces_entity_shared_ptr& entity,
                        const glm::vec2& touch_point,
                        gb::e_input_source input_source,
                        gb::e_input_state input_state);
        
        void add_touch_recognition(const gb::ces_entity_shared_ptr& entity,
                                   gb::e_input_state input_state);
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const gb::ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
    public:
        
        CTTI_CLASS_GUID(ces_interaction_system, gb::ces_base_system::g_guids_container)
        ces_interaction_system();
        ~ces_interaction_system();
    };
};
