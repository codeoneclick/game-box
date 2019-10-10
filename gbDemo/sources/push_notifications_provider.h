//
//  push_notifications_provider.h
//  gbDemo
//
//  Created by serhii.s on 10/9/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//


#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace game
{
    class push_notifications_provider
    {
    private:
        
    protected:
        
        static std::shared_ptr<push_notifications_provider> m_instance;
        
    public:
        
        push_notifications_provider();
        ~push_notifications_provider();
        
        static std::shared_ptr<push_notifications_provider> shared_instance();
        
        void authenticate();
        void schedule_day_notifications();
        void schedule_offline_notifications();
    };
};


