//
//  game_center_provier.h
//  gbDemo
//
//  Created by serhii.s on 7/31/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"
#include "declarations.h"

namespace game
{
    class game_center_provier
    {
    private:
        
    protected:
        
        static std::shared_ptr<game_center_provier> m_instance;
        
    public:
        
        game_center_provier();
        ~game_center_provier();
        
        static std::shared_ptr<game_center_provier> shared_instance();
        
        void authenticate();
        
        void rate_app();
        
        void open_drifters_leaderboard();
        void report_drifring_score(i32 seconds);
    };
};

