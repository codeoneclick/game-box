//
//  game_loop.h
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"

namespace gb
{
    class i_game_loop
    {
    private:
        
        friend class game_loop;
        bool m_paused;
        
    protected:
        
        virtual void on_update(f32 deltatime) = 0;
        
    public:
        
        i_game_loop();
        virtual ~i_game_loop() = default;
        
        void set_paused(bool paused);
        bool is_paused() const;
    };
    
    class game_loop
    {
    private:
        
        std::chrono::steady_clock::time_point m_previous_run_loop_timestamp = std::chrono::steady_clock::now();
        bool m_is_paused = false;
        
        clock_t m_delta_ticks;
        static i32 m_fps;
        static i32 m_frames_num;
        
    protected:
        
        std::set<std::shared_ptr<i_game_loop>> m_listeners;
        
    public:
        
        game_loop() = default;
        ~game_loop();
        
        void on_update();
        
        void add_listener(const std::shared_ptr<i_game_loop>& listener);
        void remove_listener(const std::shared_ptr<i_game_loop>& listener);
        
        void reset_run_loop_timestamp();
        
        void pause_run_loop();
        void resume_run_loop();
        
        static i32 get_fps();
    };
    
    void assign_hwnd_to_game_loop(const void* hwnd);
    void add_listener_to_game_loop(const std::shared_ptr<i_game_loop>& listener);
    void remove_listener_from_game_loop(const std::shared_ptr<i_game_loop>& listener);
    
    void reset_run_loop_timestamp();
    
    void pause_run_loop();
    void resume_run_loop();
    
#if defined(__OSX__)
    
    void terminate_game_loop();

#endif

#if defined(__WINOS__) || defined(__OSX__)

	extern void execute_runloop();

#endif
};
