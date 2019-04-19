
#pragma once

#include "main_headers.h"
#include "al_declarations.h"

namespace gb
{
    namespace al
    {
        class audio_profile
        {
        private:
            
        protected:
            
        public:

            std::string m_name;
            ui32 m_max_instances;
            f64 m_min_delay;

            audio_profile() :
            m_max_instances(0),
            m_min_delay(0.f)
            {
                
            }
        };
        
        class audio_engine
        {
        public:
            
            enum class e_audio_state
            {
                e_audio_state_error  = -1,
                e_audio_state_initializing,
                e_audio_state_playing,
                e_audio_state_paused
            };
            
            static const i32 k_invalid_audio_id;
            static const f32 k_time_unknown;
            
        private:
            
            class audio_engine_thread_pool;
            friend class audio_engine_impl;
            
        protected:
            
            static void add_task(const std::function<void()>& task);
            static void remove(i32 audio_id);
            
            struct profile_helper
            {
                std::shared_ptr<audio_profile> m_profile;
                std::list<i32> m_audio_ids;
                f64 m_last_play_time;
                
                profile_helper() :
                m_last_play_time(0.0),
                m_profile(std::make_shared<audio_profile>())
                {
                    
                }
            };
            
            struct audio_info
            {
                const std::string* m_filepath;
                std::shared_ptr<profile_helper> m_profile_helper;
                
                f32 m_volume;
                f32 m_pitch;
                bool m_loop;
                f32 m_duration;
                e_audio_state m_state;
                
                audio_info() :
                m_profile_helper(nullptr),
                m_duration(k_time_unknown),
                m_state(e_audio_state::e_audio_state_initializing)
                {
                    
                }
            };
            
            static std::unordered_map<i32, std::shared_ptr<audio_info>> m_audio_id_infos;
            static std::unordered_map<std::string,std::list<i32>> m_audio_path_ids;
            static std::unordered_map<std::string, std::shared_ptr<profile_helper>> m_audio_path_profile_helpers;
            
            static ui32 m_max_instances;
            
            static std::shared_ptr<profile_helper> m_default_profile_helper;
            static audio_engine_impl_shared_ptr m_audio_engine_impl;
            static std::shared_ptr<audio_engine_thread_pool> m_thread_pool;
            
        public:
            
            static bool init();
            static void end();
            
            static std::shared_ptr<audio_profile> get_default_profile();
            
            static i32 play2d(const std::string& filepath, bool loop = false, f32 volume = 1.f, f32 pitch = 1.f, const std::shared_ptr<audio_profile>& profile = nullptr);
            
            static void set_loop(i32 audio_id, bool loop);
            static bool is_loop(i32 audio_id);
            
            static void set_volume(i32 audio_id, f32 volume);
            static f32 get_volume(i32 audio_id);
            
            static void set_pitch(i32 audio_id, f32 pitch);
            static f32 get_pitch(i32 audio_id);
            
            static void pause(i32 audio_id);
            static void pause_all();
            
            static void resume(i32 audio_id);
            static void resume_all();
            
            static void stop(i32 audio_id);
            static void stop_all();
            
            static bool set_current_time(i32 audio_id, f32 seconds);
            static f32 get_current_time(i32 audio_id);
            static f32 get_duration(i32 audio_id);
            
            static e_audio_state get_state(i32 audio_id);
            
            static void set_finish_callback(i32 audio_id, const std::function<void(i32,const std::string&)>& callback);
            
            static i32 get_max_audio_instance() {return m_max_instances;}
            static bool set_max_audio_instance(i32 max_instances);
            
            static void uncache(const std::string& filepath);
            static void uncache_all();
            
            static std::shared_ptr<audio_profile> get_profile(i32 audio_id);
            static std::shared_ptr<audio_profile> get_profile(const std::string &profile_name);

            static void preload(const std::string& filepath) { preload(filepath, nullptr); }
            static void preload(const std::string& filepath, std::function<void(bool is_success)> callback);
            
        };
    };
};
