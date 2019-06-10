//
//  ces_state_automat_system.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/25/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"

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
        
        gb::ces_entity_weak_ptr m_open_levels_list_dialog_button;
        gb::ces_entity_weak_ptr m_open_garage_button;
        gb::ces_entity_weak_ptr m_back_from_garage_button;
        gb::ces_entity_weak_ptr m_levels_list_dialog;
        gb::ces_entity_weak_ptr m_next_car_in_garage_button;
        gb::ces_entity_weak_ptr m_prev_car_in_garage_button;
        gb::ces_entity_weak_ptr m_car_skin_1_button;
        gb::ces_entity_weak_ptr m_car_skin_2_button;
        gb::ces_entity_weak_ptr m_car_skin_3_button;
        gb::ces_entity_weak_ptr m_cars_list_dialog;
        
    public:
        
        CTTI_CLASS_GUID(ces_state_automat_system, gb::ces_base_system::g_guids_container)
        ces_state_automat_system();
        ~ces_state_automat_system();
    };
};
