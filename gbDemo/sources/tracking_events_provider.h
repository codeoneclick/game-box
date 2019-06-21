//
//  tracking_events_provider.h
//  gbDemo
//
//  Created by serhii.s on 6/20/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace game
{
    class tracking_events_provider
    {
    private:
        
    protected:
        
        static std::shared_ptr<tracking_events_provider> m_instance;
        
    public:
        
        tracking_events_provider();
        ~tracking_events_provider();
        
        static std::shared_ptr<tracking_events_provider> shared_instance();
        
        void on_level_enter(i32 level_id);
        void on_rank_updated(i32 rank);
        void on_car_selected(i32 car_id);
    };
};
