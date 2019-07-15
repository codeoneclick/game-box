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
        private:
        
            std::function<void()> m_on_purchases_restored = nullptr;
        
        protected:
        
            static std::shared_ptr<store_provider> m_instance;
        
        public:
        
        store_provider();
        ~store_provider();
        
        static std::shared_ptr<store_provider> shared_instance();
        
        void request_products();
        void buy_no_ads_product();
        
        void set_on_puchases_restored_callback(const std::function<void()>& callback);
        std::function<void()> get_on_puchases_restored_callback() const;
        
        bool is_no_ads_product_bought() const;
    };
};
