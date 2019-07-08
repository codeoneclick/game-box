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
        
        protected:
        
            static std::shared_ptr<store_provider> m_instance;
        
        public:
        
        store_provider();
        ~store_provider();
        
        static std::shared_ptr<store_provider> shared_instance();
        
        void request_products();
        void buy_no_ads_product();
        
    };
};
