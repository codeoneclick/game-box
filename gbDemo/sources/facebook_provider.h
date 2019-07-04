//
//  facebook_provider.h
//  gbDemo
//
//  Created by serhii.s on 7/4/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace game
{
    class facebook_provider
    {
        private:
        
        protected:
        
        static std::shared_ptr<facebook_provider> m_instance;
        
        std::function<void(bool is_error)> m_on_login = nullptr;
        std::function<void(bool is_error)> m_on_logout = nullptr;
        
        public:
        
        facebook_provider();
        ~facebook_provider();
        
        static std::shared_ptr<facebook_provider> shared_instance();
        
        void assign_root_navigation_controller(void* root_navigation_controller);
        void assign_root_view(void* view);
        
        void login();
        void logout();
        
        void set_on_login(const std::function<void(bool is_error)>& callback);
        const std::function<void(bool is_error)> get_on_login() const;
        
        void set_on_logout(const std::function<void(bool is_error)>& callback);
        const std::function<void(bool is_error)> get_on_logout() const;
    };
};

