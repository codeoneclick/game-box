//
//  ces_ui_interaction_system.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/27/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_system.h"
#include "input_context.h"

namespace game
{
    class ces_ui_interaction_system : public gb::ces_base_system
    {
    private:
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_level_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_camera_follow_car_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_ui_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_car_components_mask;
        
        gb::ces_entity_weak_ptr m_open_levels_list_dialog_button;
        gb::ces_entity_weak_ptr m_open_garage_button;
        gb::ces_entity_weak_ptr m_back_from_garage_button;
        gb::ces_entity_weak_ptr m_levels_list_dialog;
        gb::ces_entity_weak_ptr m_scores_label;
        gb::ces_entity_weak_ptr m_countdown_label;
        gb::ces_entity_weak_ptr m_next_car_in_garage_button;
        gb::ces_entity_weak_ptr m_prev_car_in_garage_button;
        gb::ces_entity_weak_ptr m_car_skin_1_button;
        gb::ces_entity_weak_ptr m_car_skin_2_button;
        gb::ces_entity_weak_ptr m_car_skin_3_button;
        gb::ces_entity_weak_ptr m_cars_list_dialog;
        
        gb::ces_entity_weak_ptr m_current_pushed_dialog;
        
        gb::ces_entity_weak_ptr m_level;
        gb::ces_entity_weak_ptr m_scene;
       
        gb::ces_entity_weak_ptr m_camera_follow_car;
        gb::ces_entity_weak_ptr m_main_car;
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_ai_cars;
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_all_cars;
        
        void on_touched(const gb::ces_entity_shared_ptr& entity,
                        const glm::vec2& touch_point,
                        gb::e_input_source input_source,
                        gb::e_input_state input_state);
        
        void add_touch_recognition(const gb::ces_entity_shared_ptr& entity,
                                   gb::e_input_state input_state);
        
        void on_dragging(const gb::ces_entity_shared_ptr&, const glm::vec2&);
        void on_drag_ended(const gb::ces_entity_shared_ptr&, const glm::vec2&);
        
        void pop_current_dialog();
        
        void push_levels_list_dialog(const gb::ces_entity_shared_ptr& root);
        void update_cars_list_dialog();
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const gb::ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
    public:
        
        CTTI_CLASS_GUID(ces_ui_interaction_system, gb::ces_base_system::g_guids_container)
        ces_ui_interaction_system();
        ~ces_ui_interaction_system() = default;
    };
};
