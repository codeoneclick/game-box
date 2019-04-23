//
//  gameplay_ui_fabricator.h
//  gbDemo
//
//  Created by serhii serhiiv on 8/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"
#include "ns_declarations.h"

namespace game
{
    class gameplay_ui_fabricator
    {
    private:
        
    protected:
        
        gb::scene_fabricator_weak_ptr m_general_fabricator;
        gb::ui::ui_fabricator_weak_ptr m_ui_base_fabricator;
        glm::ivec2 m_screen_size;
        
    public:
        
        gameplay_ui_fabricator(const gb::scene_fabricator_shared_ptr& general_fabricator,
                               const gb::ui::ui_fabricator_shared_ptr& ui_base_fabricator,
                               const glm::ivec2& screen_size);
        ~gameplay_ui_fabricator() = default;
        
        gb::game_object_2d_shared_ptr create_move_joystick(const std::string& filename);
        gb::game_object_2d_shared_ptr create_attack_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_abitily_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_character_avatar_icon(const std::string& filename);
        gb::game_object_2d_shared_ptr create_opponent_avatar_icon(const std::string& filename);
        gb::game_object_2d_shared_ptr create_quests_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_action_console(const std::string& filename);
        gb::game_object_2d_shared_ptr create_questlog_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_questlog_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_drift_time_label(const std::string& filename);
    };
};

