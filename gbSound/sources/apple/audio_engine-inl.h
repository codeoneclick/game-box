
#pragma once

#include "audio_cache.h"
#include "audio_player.h"

namespace gb
{
    namespace al
    {

#define k_max_audioinstances 24
        
        class audio_engine_impl
        {
        private:
            
            void play2d_impl(const audio_cache_shared_ptr& cache, i32 audio_id);
            
            ALuint m_al_sources[k_max_audioinstances];
            std::unordered_map<ALuint, bool> m_al_source_used;
            std::unordered_map<std::string, audio_cache_shared_ptr> m_audio_caches;
            
            std::unordered_map<i32, audio_player_shared_ptr> m_audio_players;
            std::mutex m_thread_mutex;
            
            bool m_init_loop;
            
            i32 m_current_audio_id;
            
        protected:
            
        public:
            
            audio_engine_impl();
            ~audio_engine_impl();
            
            bool init();
            
            int play2d(const std::string &filepath, bool loop, f32 volume, f32 pitch);
            
            void set_volume(i32 audio_id, f32 volume);
            void set_pitch(i32 audio_id, f32 pitch);
            
            void set_loop(i32 audio_id, bool loop);
            
            bool pause(i32 audio_id);
            bool resume(i32 audio_id);
            
            void stop(i32 audio_id);
            void stop_all();
            
            f32 get_duration(i32 audio_id);
            
            f32 get_current_time(i32 audio_id);
            bool set_current_time(i32 audio_id, f32 time);
            
            void set_finish_callback(i32 audioID, const std::function<void(i32, const std::string &)> &callback);
            
            void uncache(const std::string& filepath);
            void uncache_all();
            
            audio_cache_shared_ptr preload(const std::string& filepath, std::function<void(bool)> callback);
            void update(f32 dt);
        };
    };
};
