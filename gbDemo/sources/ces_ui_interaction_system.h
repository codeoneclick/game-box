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
        
        enum e_character_move_state
        {
            e_character_move_state_none = 0,
            e_character_move_state_forward,
            e_character_move_state_backward
        };
        
        enum e_character_steer_state
        {
            e_character_steer_state_none = 0,
            e_character_steer_state_left,
            e_character_steer_state_right
        };
        
        e_character_move_state m_character_move_state = e_character_move_state::e_character_move_state_none;
        e_character_steer_state m_character_steer_state = e_character_steer_state::e_character_steer_state_none;
        
        std::bitset<std::numeric_limits<uint8_t>::max()> m_level_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_ui_components_mask;
        std::bitset<std::numeric_limits<uint8_t>::max()> m_character_components_mask;
        gb::ces_entity_weak_ptr m_move_joystick;
        gb::ces_entity_weak_ptr m_attack_button;
        gb::ces_entity_weak_ptr m_quests_dialog;
        gb::ces_entity_weak_ptr m_action_console;
        gb::ces_entity_weak_ptr m_questlog_button;
        gb::ces_entity_weak_ptr m_questlog_dialog;
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_abilities_buttons;
        gb::ces_entity_weak_ptr m_character_avatar_icon;
        gb::ces_entity_weak_ptr m_opponent_avatar_icon;
        gb::ces_entity_weak_ptr m_level;
        gb::ces_entity_weak_ptr m_main_character;
        std::unordered_map<std::string, gb::ces_entity_weak_ptr> m_all_characters;
        
        void on_touched(const gb::ces_entity_shared_ptr& entity,
                        const glm::vec2& touch_point,
                        gb::e_input_source input_source,
                        gb::e_input_state input_state);
        
        void add_touch_recognition(const gb::ces_entity_shared_ptr& entity,
                                   gb::e_input_state input_state);
        
        void on_dragging(const gb::ces_entity_shared_ptr&, const glm::vec2&);
        void on_drag_ended(const gb::ces_entity_shared_ptr&, const glm::vec2&);
        
        void show_quests_dialog();
        void show_questlog_dialog();
        
    protected:
        
        void on_feed_start(f32 dt);
        void on_feed(const gb::ces_entity_shared_ptr& root, f32 dt);
        void on_feed_end(f32 dt);
        
    public:
        
        CTTI_CLASS_GUID(ces_ui_interaction_system, gb::ces_base_system::g_guids_container)
        ces_ui_interaction_system();
        ~ces_ui_interaction_system();
    };
};
