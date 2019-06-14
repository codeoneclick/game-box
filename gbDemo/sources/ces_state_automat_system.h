//
//  ces_state_automat_system.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/25/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"
#include "ces_ui_interaction_component.h"

namespace game
{
    class ces_state_automat_system : public gb::ces_base_system
    {
    private:
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_state_automat_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_level_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_main_car_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_ai_car_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_ui_components_mask;
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const gb::ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
        gb::ces_entity_weak_ptr m_level;
        gb::ces_entity_weak_ptr m_main_car;
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_all_cars;
        
    public:
        
        STTI_CLASS_GUID(ces_state_automat_system, gb::ces_base_system::g_guids_container)
        ces_state_automat_system();
        ~ces_state_automat_system();
    };
};
