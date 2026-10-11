//
//  game_loop.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "game_loop.h"
#include "thread_operations_pool.h"

namespace gb
{
    
    i32 game_loop::m_fps = 30;
    i32 game_loop::m_frames_num = 0;
    
    i_game_loop::i_game_loop() :
    m_paused(false)
    {
        
    }
    
    void i_game_loop::set_paused(bool paused)
    {
        m_paused = paused;
    }
    
    bool i_game_loop::is_paused() const
    {
        return m_paused;
    }
    
    game_loop::~game_loop()
    {
        m_listeners.clear();
    }
    
    void game_loop::on_update()
    {
        std::chrono::steady_clock::time_point current_run_loop_timestamp = std::chrono::steady_clock::now();
        const double elapsed = std::chrono::duration<double>(current_run_loop_timestamp - m_previous_run_loop_timestamp).count();
        const f32 dt = static_cast<f32>(elapsed);
        m_previous_run_loop_timestamp = current_run_loop_timestamp;
        if (!m_is_paused)
        {
            for(const auto& listener : m_listeners)
            {
                if(!listener->is_paused())
                {
                    listener->on_update(dt);
                }
            }
        }
        thread_operations_pool::shared_instance()->update();
        
        m_fps_elapsed += elapsed;
        m_frames_num++;
        if (m_fps_elapsed >= 1.0)
        {
            m_fps = static_cast<i32>(m_frames_num / m_fps_elapsed + .5);
            m_fps_elapsed = 0.0;
            m_frames_num = 0;
        }
    }
    
    void game_loop::add_listener(const std::shared_ptr<i_game_loop>& listener)
    {
        m_listeners.insert(listener);
    }
    
    void game_loop::remove_listener(const std::shared_ptr<i_game_loop>& listener)
    {
        m_listeners.erase(listener);
    }
    
    void game_loop::reset_run_loop_timestamp()
    {
        m_previous_run_loop_timestamp = std::chrono::steady_clock::now();
        m_fps_elapsed = 0.0;
        m_frames_num = 0;
    }
    
    void game_loop::pause_run_loop()
    {
        m_is_paused = true;
        reset_run_loop_timestamp();
    }
    
    void game_loop::resume_run_loop()
    {
        m_is_paused = false;
        reset_run_loop_timestamp();
    }
    
    i32 game_loop::get_fps()
    {
        return m_fps;
    }
}
