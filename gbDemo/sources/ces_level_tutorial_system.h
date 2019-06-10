//
//  ces_level_tutorial_system.h
//  gbDemo
//
//  Created by serhii.s on 5/24/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"
#include "ns_declarations.h"

namespace game
{
    class ces_level_tutorial_system : public gb::ces_base_system
    {
    private:
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_level_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_cars_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_ui_components_mask;
        
        gb::ces_entity_weak_ptr m_level;
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_cars;
        gb::ces_entity_weak_ptr m_main_car;
        
        gb::ces_entity_weak_ptr m_tutorial_steer_left_label;
        gb::ces_entity_weak_ptr m_tutorial_steer_right_label;
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const gb::ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
    public:
        
        CTTI_CLASS_GUID(ces_level_tutorial_system, gb::ces_base_system::g_guids_container)
        ces_level_tutorial_system();
        ~ces_level_tutorial_system() = default;
    };
};
