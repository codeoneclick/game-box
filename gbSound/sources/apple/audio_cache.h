
#pragma once

#include "main_headers.h"
#include "al_declarations.h"
#include <OpenAL/al.h>
#include <AudioToolbox/AudioToolbox.h>

namespace gb
{
    namespace al
    {
        
#define k_queue_buffer_num 3
#define k_queue_buffer_time_step 0.1
        
        class audio_cache
        {
        private:
            
            friend class audio_engine_impl;
            friend class audio_player;
            
        protected:
            
            void read_data_task();
            
            void invoking_play_callbacks();
            void invoking_load_callbacks();
            
            ALsizei m_data_size;
            ALenum m_format;
            ALsizei m_sample_rate;
            
            f32 m_duration;
            i32 m_bytes_per_frame;
            
            AudioStreamBasicDescription m_output_format;
            
            ALuint m_al_buffer_id;
            char* m_pcm_data;
            i64 m_bytes_of_read;
            
            char* m_queue_buffers[k_queue_buffer_num];
            ALsizei m_queue_buffer_size[k_queue_buffer_num];
            ui32 m_queue_buffer_frames;
            ui32 m_queue_buffer_bytes;
            
            bool m_al_buffer_ready;
            bool m_load_fail;
            std::mutex m_callback_mutex;
            
            std::vector<std::function<void()>> m_callbacks;
            std::vector<std::function<void(bool)>> m_load_callbacks;
            std::mutex m_read_data_task_mutex;
            
            bool m_exit_read_data_task;
            std::string m_filepath;
            
        public:
            
            audio_cache();
            ~audio_cache();
            
            void add_play_callback(const std::function<void()>& callback);
            void add_load_callback(const std::function<void(bool)>& callback);
        };
    };
};


