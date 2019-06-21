//
//  cars_list_table_view_cell.h
//  gbDemo
//
//  Created by serhii.s on 5/21/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "table_view_cell.h"

namespace game
{
    namespace ui
    {
        class cars_list_table_view_cell_data : public gb::ui::table_view_cell_data
        {
        private:
            
        protected:
            
            std::string m_id = "";
            std::string m_name = "unknown";
            i32 m_place = -1;
            f32 m_drift_time = 0.f;
            
        public:
            
            cars_list_table_view_cell_data();
            ~cars_list_table_view_cell_data() = default;
            
            std::property_rw<std::string> id;
            std::property_rw<std::string> name;
            std::property_rw<i32> place;
            std::property_rw<f32> drift_time;
        };
        
        class cars_list_table_view_cell : public gb::ui::table_view_cell
        {
        public:
            
            static const std::string k_name_label_id;
            static const std::string k_place_label_id;
            static const std::string k_drift_time_label_id;
            static const std::string k_drift_time_value_label_id;
            
        private:
            
        protected:
            
        public:
            
            cars_list_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier);
            ~cars_list_table_view_cell() = default;
            
            std::property_rw<std::string> name;
            std::property_rw<i32> place;
            std::property_rw<f32> drift_time;
            
            void setup_components() override;
            void create() override;
        };
    };
};

