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
    class events_provider
    {
    private:
        
    protected:
        
        static std::shared_ptr<events_provider> m_instance;
        
    public:
        
        events_provider();
        ~events_provider();
        
        static std::shared_ptr<events_provider> shared_instance();
        
        void on_level_enter(i32 level_id);
        void on_level_finished(i32 level_id, i32 first_place_achievement, i32 low_damage_achievement, i32 good_drift_achievement, i32 retries_count);
        void on_car_damaged(i32 level_id, i32 retries_count);
        void on_rank_updated(i32 rank);
        void on_car_selected(i32 car_id);
    };
};
