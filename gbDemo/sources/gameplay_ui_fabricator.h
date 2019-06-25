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
    public:
        
        static const glm::u8vec4 k_font_color;
        static const glm::u8vec4 k_image_button_color;
        static const glm::u8vec4 k_control_background_color;
        static const glm::u8vec4 k_control_foreground_color;
        
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
        
        glm::ivec2 get_screen_size() const;
        
        gb::game_object_2d_shared_ptr create_open_levels_list_dialog_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_open_garage_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_back_from_garage_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_next_car_in_garage_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_prev_car_in_garage_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_levels_list_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_countdown_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_car_skin_1_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_car_skin_2_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_car_skin_3_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_cars_list_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_tutorial_steer_left_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_tutorial_steer_right_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_goto_racing_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_in_game_pause_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_in_game_pause_menu_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_in_game_restart_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_in_game_quit_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_tickets_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_win_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_loose_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_select_car_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_unlock_car_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_stars_progress_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_stars_progress_bar(const std::string& filename);
        gb::game_object_2d_shared_ptr create_stars_progress_info_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_stars_progress_button(const std::string& filename);
        gb::game_object_2d_shared_ptr create_rank_info_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_car_damage_label(const std::string& filename);
        gb::game_object_2d_shared_ptr create_car_damage_bar(const std::string& filename);
        gb::game_object_2d_shared_ptr create_full_tickets_dialog(const std::string& filename);
        gb::game_object_2d_shared_ptr create_not_enough_tickets_dialog(const std::string& filename);
    };
};

