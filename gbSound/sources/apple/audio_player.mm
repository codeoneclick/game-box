
#include "audio_player.h"
#include "audio_cache.h"
#include <Foundation/Foundation.h>
#include <AudioToolbox/ExtendedAudioFile.h>

namespace gb
{
    namespace al
    {
        audio_player::audio_player() :
        m_audio_cache(nullptr),
        m_finish_callbak(nullptr),
        m_be_destroy(false),
        m_remove_by_audio_engine(false),
        m_ready(false),
        m_current_time(0.f),
        m_streaming_source(false),
        m_time_dirty(false)
        {
        }
        
        audio_player::~audio_player()
        {
            if (m_streaming_source)
            {
                m_be_destroy = true;
                m_sleep_condition.notify_one();
                if (m_rotate_buffer_thread.joinable())
                {
                    m_rotate_buffer_thread.join();
                }
                alDeleteBuffers(3, m_buffer_ids);
            }
        }
        
        void audio_player::destroy()
        {
            alSourceStop(m_al_source);
            alSourcei(m_al_source, AL_BUFFER, NULL);
            
            std::unique_lock<std::mutex> lock(m_sleep_mutex);
            m_be_destroy = true;
            if(m_streaming_source)
            {
                m_sleep_condition.notify_one();
            }
            else if(m_ready)
            {
                m_remove_by_audio_engine = true;
            }
            m_ready = false;
        }
        
        bool audio_player::play2d(const audio_cache_shared_ptr& cache)
        {
            if (!cache->m_al_buffer_ready)
            {
                m_remove_by_audio_engine = true;
                return false;
            }
            m_audio_cache = cache;
            
            alSourcei(m_al_source, AL_BUFFER, 0);
            alSourcef(m_al_source, AL_PITCH, m_pitch);
            alSourcef(m_al_source, AL_GAIN, m_volume);
            alSourcei(m_al_source, AL_LOOPING, AL_FALSE);
            
            if (m_audio_cache->m_queue_buffer_frames == 0)
            {
                if(m_loop)
                {
                    alSourcei(m_al_source, AL_LOOPING, AL_TRUE);
                }
            }
            else
            {
                auto al_error = alGetError();
                alGenBuffers(3, m_buffer_ids);
                al_error = alGetError();
                if(al_error == AL_NO_ERROR)
                {
                    for (int index = 0; index < k_queue_buffer_num; ++index)
                    {
                        alBufferData(m_buffer_ids[index], m_audio_cache->m_format, m_audio_cache->m_queue_buffers[index], m_audio_cache->m_queue_buffer_size[index], m_audio_cache->m_sample_rate);
                    }
                }
                else
                {
                    m_remove_by_audio_engine = true;
                    return false;
                }
                m_streaming_source = true;
            }
            
            {
                std::unique_lock<std::mutex> lock(m_sleep_mutex);
                if(m_be_destroy)
                {
                    m_remove_by_audio_engine = true;
                    return false;
                }
                
                if(m_streaming_source)
                {
                    alSourceQueueBuffers(m_al_source, k_queue_buffer_num, m_buffer_ids);
                    m_rotate_buffer_thread = std::thread(&audio_player::rotate_buffer_thread, this, m_audio_cache->m_queue_buffer_frames * k_queue_buffer_num + 1);
                }
                else
                {
                    alSourcei(m_al_source, AL_BUFFER, m_audio_cache->m_al_buffer_id);
                }
                
                auto al_error = alGetError();
                if (al_error == AL_NO_ERROR)
                {
                    alSourcePlay(m_al_source);
                }
            }
            
            auto al_error = alGetError();
            if (al_error != AL_NO_ERROR)
            {
                m_remove_by_audio_engine = true;
                return false;
            }
            m_ready = true;
            return true;
        }
        
        void audio_player::rotate_buffer_thread(i32 offset_frame)
        {
            ALint source_state;
            ALint buffer_processed = 0;
            ExtAudioFileRef external_reference = nullptr;
            
            NSString *filepath = [[NSString alloc] initWithCString:m_audio_cache->m_filepath.c_str() encoding:[NSString defaultCStringEncoding]];
            NSURL *file_url = [[NSURL alloc] initFileURLWithPath:filepath];
            CFURLRef core_file_url = (__bridge CFURLRef)file_url;

            char* buffer = (char*)malloc(m_audio_cache->m_queue_buffer_bytes);
            auto frames = m_audio_cache->m_queue_buffer_frames;
            
            auto error = ExtAudioFileOpenURL(core_file_url, &external_reference);
            if(error)
            {
                goto EXIT_THREAD;
            }
            
            error = ExtAudioFileSetProperty(external_reference, kExtAudioFileProperty_ClientDataFormat, sizeof(m_audio_cache->m_output_format), &m_audio_cache->m_output_format);
            AudioBufferList	data_buffer;
            data_buffer.mNumberBuffers = 1;
            data_buffer.mBuffers[0].mData = buffer;
            data_buffer.mBuffers[0].mDataByteSize = m_audio_cache->m_queue_buffer_bytes;
            data_buffer.mBuffers[0].mNumberChannels = m_audio_cache->m_output_format.mChannelsPerFrame;
            
            if (offset_frame != 0)
            {
                ExtAudioFileSeek(external_reference, offset_frame);
            }
            
            while(!m_be_destroy)
            {
                alGetSourcei(m_al_source, AL_SOURCE_STATE, &source_state);
                if (source_state == AL_PLAYING)
                {
                    alGetSourcei(m_al_source, AL_BUFFERS_PROCESSED, &buffer_processed);
                    while (buffer_processed > 0)
                    {
                        buffer_processed--;
                        if(m_time_dirty)
                        {
                            m_time_dirty = false;
                            offset_frame = m_current_time * m_audio_cache->m_output_format.mSampleRate;
                            ExtAudioFileSeek(external_reference, offset_frame);
                        }
                        else
                        {
                            m_current_time += k_queue_buffer_time_step;
                            if(m_current_time > m_audio_cache->m_duration)
                            {
                                if(m_loop)
                                {
                                    m_current_time = 0.f;
                                }
                                else
                                {
                                    m_current_time = m_audio_cache->m_duration;
                                }
                            }
                        }
                        
                        frames = m_audio_cache->m_queue_buffer_frames;
                        ExtAudioFileRead(external_reference, (UInt32*)&frames, &data_buffer);
                        if(frames <= 0)
                        {
                            if(m_loop)
                            {
                                ExtAudioFileSeek(external_reference, 0);
                                frames = m_audio_cache->m_queue_buffer_frames;
                                data_buffer.mBuffers[0].mDataByteSize = m_audio_cache->m_queue_buffer_bytes;
                                ExtAudioFileRead(external_reference, (UInt32*)&frames, &data_buffer);
                            }
                            else
                            {
                                m_be_destroy = true;
                                break;
                            }
                        }
                        
                        ALuint buffer_id;
                        alSourceUnqueueBuffers(m_al_source, 1, &buffer_id);
                        alBufferData(buffer_id, m_audio_cache->m_format, buffer, frames * m_audio_cache->m_output_format.mBytesPerFrame, m_audio_cache->m_sample_rate);
                        alSourceQueueBuffers(m_al_source, 1, &buffer_id);
                    }
                }
                
                std::unique_lock<std::mutex> lock(m_sleep_mutex);
                if (m_be_destroy)
                {
                    break;
                }
                
                m_sleep_condition.wait_for(lock,std::chrono::milliseconds(1));
            }
            
        EXIT_THREAD:
            
            if (external_reference)
            {
                ExtAudioFileDispose(external_reference);
            }
            free(buffer);
        }
        
        bool audio_player::set_loop(bool loop)
        {
            if(!m_be_destroy)
            {
                m_loop = loop;
                return true;
            }
            return false;
        }
        
        bool audio_player::set_time(f32 time)
        {
            if (!m_be_destroy && time >= 0.0f && time < m_audio_cache->m_duration)
            {
                m_current_time = time;
                m_time_dirty = true;
                return true;
            }
            return false;
        }
    }
}
