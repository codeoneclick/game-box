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
        
        std::function<void()> m_on_reward_video_viewed = nullptr;
        std::function<void()> m_on_video_ended = nullptr;
        
    public:
        
        advertisement_provider();
        ~advertisement_provider();
        
        static std::shared_ptr<advertisement_provider> shared_instance();
        
        void assign_root_navigation_controller(void* root_navigation_controller);
        void assign_root_view(void* view);
        
        bool play_rewarded_video();
        
        bool play_interstitial_video();
        
        void show_banner();
        void hide_banner();
        bool is_banner_shown() const;
        
        void set_on_reward_video_viewed(const std::function<void()>& callback);
        const std::function<void()> get_on_reward_video_viewed() const;
        
        void set_on_video_ended(const std::function<void()>& callback);
        const std::function<void()> get_on_video_ended() const;
    };
};
