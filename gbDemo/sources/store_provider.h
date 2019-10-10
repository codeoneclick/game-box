//
//  store_provider.hpp
//  gbDemo
//
//  Created by serhii.s on 7/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace game
{
    class store_provider
    {
    public:
        
        static const i32 k_no_ads_product_id;
        static const i32 k_cash_pack_1_product_id;
        static const i32 k_cash_pack_2_product_id;
        static const i32 k_cash_pack_3_product_id;
        static const i32 k_vip_subscription_product_id;
        
    private:
        
        std::function<void(i32)> m_on_purchases_restored = nullptr;
        std::function<void(bool)> m_on_subscription_status_changed = nullptr;
        
        std::function<void(bool)> m_on_purchase_no_ads = nullptr;
        std::function<void(bool)> m_on_purchase_small_cash_pack = nullptr;
        std::function<void(bool)> m_on_purchase_medium_cash_pack = nullptr;
        std::function<void(bool)> m_on_purchase_big_cash_pack = nullptr;
        std::function<void(bool)> m_on_purchase_vip_subscription = nullptr;
        
    protected:
        
        static std::shared_ptr<store_provider> m_instance;
        
    public:
        
        store_provider();
        ~store_provider();
        
        static std::shared_ptr<store_provider> shared_instance();
        
        void request_products();
        void buy_no_ads_product(const std::function<void(bool)>& callback);
        void buy_small_cash_pack(const std::function<void(bool)>& callback);
        void buy_medium_cash_pack(const std::function<void(bool)>& callback);
        void buy_big_cash_pack(const std::function<void(bool)>& callback);
        void buy_vip_subscription(const std::function<void(bool)>& callback);
        
        void restore_products();
        
        void set_on_puchases_restored_callback(const std::function<void(i32)>& callback);
        std::function<void(i32)> get_on_puchases_restored_callback() const;
        
        void set_on_subscription_status_changed_callback(const std::function<void(bool)>& callback);
        std::function<void(bool)> get_on_subscription_status_changed_callback() const;
        
        std::function<void(bool)> get_on_puchase_no_ads_callback() const;
        std::function<void(bool)> get_on_puchase_small_cash_pack_callback() const;
        std::function<void(bool)> get_on_puchase_medium_cash_pack_callback() const;
        std::function<void(bool)> get_on_puchase_big_cash_pack_callback() const;
        std::function<void(bool)> get_on_puchase_vip_subscription_callback() const;
        
        bool is_no_ads_product_bought() const;
    };
};
