//
//  levels_list_table_view_cell.h
//  gbDemo
//
//  Created by serhii.s on 5/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "table_view_cell.h"

namespace game
{
    namespace ui
    {
        class levels_list_table_view_cell_data : public gb::ui::table_view_cell_data
        {
        private:
            
        protected:
            
            i32 m_id = -1;
            std::string m_name = "level";
            i32 m_stars_count = 0;
            f32 m_drift_time = 0.f;
            bool m_is_locked = false;
            bool m_is_passed = false;
            
        public:
            
            levels_list_table_view_cell_data();
            ~levels_list_table_view_cell_data() = default;
            
            std::property_rw<i32> id;
            std::property_rw<std::string> name;
            std::property_rw<i32> stars_count;
            std::property_rw<bool> is_locked;
            std::property_rw<bool> is_passed;
            std::property_rw<f32> drift_time;
        };
        
        class levels_list_table_view_cell : public gb::ui::table_view_cell
        {
        public:
            
            typedef std::function<void(i32)> start_level_button_callback_t;
            
            static const std::string k_start_level_button_id;
            static const std::string k_show_replay_level_button_id;
            static const std::string k_level_index_label_id;
            static const std::string k_locked_unlocked_image_id;
            static const std::string k_star1_image_id;
            static const std::string k_star2_image_id;
            static const std::string k_star3_image_id;
            static const std::string k_drift_time_label_id;
            static const std::string k_drift_time_value_label_id;
            
        private:
            
        protected:
            
            start_level_button_callback_t m_start_level_button_callback;
            
        public:
            
            levels_list_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier);
            ~levels_list_table_view_cell() = default;
            
            void setup_components() override;
            void create() override;
            
            void set_start_level_button_callback_t(const start_level_button_callback_t& callback);
            
            void set_index(i32 index);
            void set_is_locked(bool value);
            void set_is_passed(bool value);
            void set_stars_count(i32 value);
            void set_drift_time(f32 value);
        };
    };
};
