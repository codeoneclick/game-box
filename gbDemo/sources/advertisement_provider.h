//
//  advertisement_provider.h
//  gbDemo
//
//  Created by serhii.s on 4/17/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace game
{
    class advertisement_provider
    {
    private:
        
    protected:
        
        static std::shared_ptr<advertisement_provider> m_instance;
        
    public:
        
        advertisement_provider();
        ~advertisement_provider();
        
        static std::shared_ptr<advertisement_provider> shared_instance();
        
        void assign_root_navigation_controller(void* root_navigation_controller);
        void assign_root_view(void* view);
        
        void play_rewarded_video();
        
        void show_banner();
    };
};
