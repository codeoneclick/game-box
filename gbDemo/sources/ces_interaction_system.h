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
        
        enum class e_move_state
        {
            e_none = 0,
            e_forward,
            e_backward
        };
        
        enum class e_steer_state
        {
            e_none = 0,
            e_right,
            e_left
        };
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_level_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_car_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_camera_follow_car_components_mask;
        gb::ces_entity_weak_ptr m_level;
        gb::ces_entity_weak_ptr m_main_car;
        gb::ces_entity_weak_ptr m_camera_follow_car;
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_ai_cars;
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_all_cars;
        
        bool m_is_interacted = false;
        glm::vec2 m_interaction_point = glm::vec2(0.f);
        glm::vec2 m_first_interaction_point = glm::vec2(0.f);
        
        e_move_state m_move_state = e_move_state::e_none;
        e_steer_state m_steer_state = e_steer_state::e_none;
        
        void on_touched(const gb::ces_entity_shared_ptr& entity,
                        const glm::vec2& touch_point,
                        gb::e_input_source input_source,
                        gb::e_input_state input_state);
        
        void on_key(const gb::ces_entity_shared_ptr& entity,
                    gb::e_input_state input_state,
                    i32 key);
        
        void add_touch_recognition(const gb::ces_entity_shared_ptr& entity,
                                   gb::e_input_state input_state);
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const gb::ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
    public:
        
        STTI_CLASS_GUID(ces_interaction_system, gb::ces_base_system::g_guids_container)
        ces_interaction_system();
        ~ces_interaction_system() = default;
    };
};
