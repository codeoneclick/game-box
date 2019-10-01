//
//  daily_task_table_view_cell.h
//  gbDemo
//
//  Created by serhii.s on 9/30/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "table_view_cell.h"

namespace game
{
    namespace ui
    {
        class daily_task_table_view_cell_data : public gb::ui::table_view_cell_data
        {
        private:
            
        protected:
            
            i32 m_id = -1;
            std::string m_name = "Name";
            std::string m_description = "Description";
            f32 m_progress = 0.f;
            bool m_is_done = false;
            bool m_is_claimed = false;
            i32 m_cash_reward = 0;
            
        public:
            
            daily_task_table_view_cell_data();
            ~daily_task_table_view_cell_data() = default;
            
            std::property_rw<i32> id;
            std::property_rw<std::string> name;
            std::property_rw<std::string> description;
            std::property_rw<f32> progress;
            std::property_rw<bool> is_done;
            std::property_rw<bool> is_claimed;
            std::property_rw<i32> cash_reward;
        };
        
        class daily_task_table_view_cell : public gb::ui::table_view_cell
        {
        public:
            
            typedef std::function<void(i32)> claim_reward_button_callback_t;
            
            static const std::string k_claim_reward_button_id;
            static const std::string k_name_label_id;
            static const std::string k_description_label_id;
            static const std::string k_reward_label_id;
            static const std::string k_progress_bar_id;
            
        private:
            
        protected:
            
            claim_reward_button_callback_t m_claim_reward_button_callback;
            
        public:
            
            daily_task_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier);
            ~daily_task_table_view_cell() = default;
            
            void setup_components() override;
            void create() override;
            
            void set_claim_reward_button_callback_t(const claim_reward_button_callback_t& callback);
            
            void set_name(const std::string& name);
            void set_description(const std::string& description);
            void set_progress(f32 value);
            void set_is_done(bool value);
            void set_is_claimed(bool value);
            void set_cash_reward(i32 value);
        };
    };
};

