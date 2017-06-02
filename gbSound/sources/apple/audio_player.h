#pragma once

#include "main_headers.h"
#include "al_declarations.h"
#include <OpenAL/al.h>

namespace gb
{
    namespace al
    {
       
        
        class audio_player
        {
        private:
            
            friend class audio_engine_impl;
            
        protected:
            
            void rotate_buffer_thread(i32 offset_frame);
            bool play2d(audio_cache_shared_ptr cache);
            
            audio_cache_shared_ptr m_audio_cache;
            
            f32 m_volume;
            bool m_loop;
            std::function<void(i32, const std::string &)> m_finish_callbak;
            
            bool m_be_destroy;
            bool m_remove_by_audio_engine;
            bool m_ready;
            ALuint m_al_source;
            
            f32 m_current_time;
            bool m_streaming_source;
            ALuint m_buffer_ids[3];
            std::thread m_rotate_buffer_thread;
            std::condition_variable m_sleep_condition;
            std::mutex m_sleep_mutex;
            bool m_time_dirty;
            
            
            
        public:
            
            audio_player();
            ~audio_player();
            
            void destroy();

            bool set_time(f32 time);
            f32 get_time() { return m_current_time;}
            
            bool set_loop(bool loop);
        };
    };
};
