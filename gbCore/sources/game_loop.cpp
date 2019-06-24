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
        f32 dt = std::chrono::duration_cast<std::chrono::milliseconds>(current_run_loop_timestamp - m_previous_run_loop_timestamp).count() / 1000.f;
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
    }
    
    void game_loop::pause_run_loop()
    {
        m_is_paused = true;
    }
    
    void game_loop::resume_run_loop()
    {
        m_is_paused = false;
    }
}
