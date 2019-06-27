//
//  shop_table_view_cell.hpp
//  gbDemo
//
//  Created by serhii.s on 6/26/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "table_view_cell.h"

namespace game
{
    namespace ui
    {
        class shop_table_view_cell_data : public gb::ui::table_view_cell_data
        {
        private:
            
        protected:
            
            i32 m_id = -1;
            std::string m_product_name = "product";
            std::string m_product_description = "product description";
            f32 m_product_price = .99f;
            bool m_is_bought = false;
            
        public:
            
            shop_table_view_cell_data();
            ~shop_table_view_cell_data() = default;
            
            std::property_rw<i32> id;
            std::property_rw<std::string> product_name;
            std::property_rw<std::string> product_description;
            std::property_rw<f32> product_price;
            std::property_rw<bool> is_bought;
        };
        
        class shop_table_view_cell : public gb::ui::table_view_cell
        {
        public:
            
            typedef std::function<void(i32)> buy_product_button_callback_t;
            
            static const std::string k_buy_product_button_id;
            static const std::string k_product_name_label_id;
            static const std::string k_product_description_label_id;
            static const std::string k_product_price_label_id;
            
        private:
            
        protected:
            
            buy_product_button_callback_t m_buy_product_button_callback;
            
        public:
            
            shop_table_view_cell(const gb::scene_fabricator_shared_ptr& fabricator, i32 index, const std::string& identifier);
            ~shop_table_view_cell() = default;
            
            void setup_components() override;
            void create() override;
            
            void set_buy_product_button_callback(const buy_product_button_callback_t& callback);
            
            void set_product_name(const std::string& value);
            void set_product_description(const std::string& value);
            void set_product_price(f32 value);
            void set_is_bought(bool value);
        };
    };
};

