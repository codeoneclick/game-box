
#include "audio_cache.h"
#include "thread_operation.h"
#include <Foundation/Foundation.h>
#include <OpenAL/alc.h>
#include <AudioToolbox/ExtendedAudioFile.h>

namespace gb
{
    namespace al
    {
        
#define k_pcm_data_cache_max_size 1048576
        
        typedef ALvoid	AL_APIENTRY	(*al_buffer_data_proc_ptr) (const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq);
        static ALvoid al_buffer_data_proc(const ALint bid, ALenum format, ALvoid* data, ALsizei size, ALsizei freq)
        {
            static al_buffer_data_proc_ptr proc_ptr = nullptr;
            if (proc_ptr == nullptr)
            {
                proc_ptr = (al_buffer_data_proc_ptr)alcGetProcAddress(nullptr, (const ALCchar*) "alBufferDataStatic");
            }
            
            if (proc_ptr)
            {
                proc_ptr(bid, format, data, size, freq);
            }
        }
        
        audio_cache::audio_cache() :
        m_data_size(0),
        m_pcm_data(nullptr),
        m_bytes_of_read(0),
        m_queue_buffer_frames(0),
        m_queue_buffer_bytes(0),
        m_al_buffer_ready(false),
        m_load_fail(false),
        m_exit_read_data_task(false)
        {
            
        }
        
        audio_cache::~audio_cache()
        {
            m_exit_read_data_task = true;
            if(m_pcm_data)
            {
                if(m_al_buffer_ready)
                {
                    alDeleteBuffers(1, &m_al_buffer_id);
                }
                m_read_data_task_mutex.lock();
                m_read_data_task_mutex.unlock();
                
                free(m_pcm_data);
            }
            
            if (m_queue_buffer_frames > 0)
            {
                for (i32 index = 0; index < k_queue_buffer_num; ++index)
                {
                    free(m_queue_buffers[index]);
                }
            }
        }
        
        void audio_cache::read_data_task()
        {
            m_read_data_task_mutex.lock();
            
            AudioStreamBasicDescription file_format;
            UInt32 property_size = sizeof(file_format);
            
            SInt64 file_length_in_frames;
            SInt64 read_in_frames;
            SInt64 data_size;
            SInt64 frames;
            AudioBufferList data_buffer;
            ExtAudioFileRef external_reference = nullptr;
            
            NSString *filepath = [[NSString alloc] initWithCString:m_filepath.c_str() encoding:[NSString defaultCStringEncoding]];
            NSURL *file_url = [[NSURL alloc] initFileURLWithPath:filepath];
            CFURLRef core_file_url = (__bridge CFURLRef)file_url;

            auto error = ExtAudioFileOpenURL(core_file_url, &external_reference);
            if(error)
            {
                goto EXIT_THREAD;
            }
            
            error = ExtAudioFileGetProperty(external_reference, kExtAudioFileProperty_FileDataFormat, &property_size, &file_format);
            if(error)
            {
                goto EXIT_THREAD;
            }
            if (file_format.mChannelsPerFrame > 2)
            {
                goto EXIT_THREAD;
            }
            
            m_output_format.mSampleRate = file_format.mSampleRate;
            m_output_format.mChannelsPerFrame = file_format.mChannelsPerFrame;
            
            m_bytes_per_frame = 2 * m_output_format.mChannelsPerFrame;
            m_output_format.mFormatID = kAudioFormatLinearPCM;
            m_output_format.mBytesPerPacket = m_bytes_per_frame;
            m_output_format.mFramesPerPacket = 1;
            m_output_format.mBytesPerFrame = m_bytes_per_frame;
            m_output_format.mBitsPerChannel = 16;
            m_output_format.mFormatFlags = kAudioFormatFlagsNativeEndian | kAudioFormatFlagIsPacked | kAudioFormatFlagIsSignedInteger;
            
            error = ExtAudioFileSetProperty(external_reference, kExtAudioFileProperty_ClientDataFormat, sizeof(m_output_format), &m_output_format);
            if(error)
            {
                goto EXIT_THREAD;
            }
            
            property_size = sizeof(file_length_in_frames);
            error = ExtAudioFileGetProperty(external_reference, kExtAudioFileProperty_FileLengthFrames, &property_size, &file_length_in_frames);
            if(error)
            {
                goto EXIT_THREAD;
            }
            
            m_data_size = (ALsizei)(file_length_in_frames * m_output_format.mBytesPerFrame);
            m_format = (m_output_format.mChannelsPerFrame > 1) ? AL_FORMAT_STEREO16 : AL_FORMAT_MONO16;
            m_sample_rate = (ALsizei)m_output_format.mSampleRate;
            m_duration = 1.f * file_length_in_frames / m_output_format.mSampleRate;
            
            if (m_data_size <= k_pcm_data_cache_max_size)
            {
                m_pcm_data = (char*)malloc(m_data_size);
                alGenBuffers(1, &m_al_buffer_id);
                auto al_error = alGetError();
                if (al_error != AL_NO_ERROR)
                {
                    goto EXIT_THREAD;
                }
                al_buffer_data_proc(m_al_buffer_id, m_format, m_pcm_data, m_data_size, m_sample_rate);
                
                read_in_frames = file_format.mSampleRate * k_queue_buffer_time_step * k_queue_buffer_num;
                data_size = m_output_format.mBytesPerFrame * read_in_frames;
                if (data_size > m_data_size)
                {
                    data_size = m_data_size;
                    read_in_frames = file_length_in_frames;
                }
                data_buffer.mNumberBuffers = 1;
                data_buffer.mBuffers[0].mDataByteSize = (UInt32)data_size;
                data_buffer.mBuffers[0].mNumberChannels = m_output_format.mChannelsPerFrame;
                
                data_buffer.mBuffers[0].mData = m_pcm_data;
                frames = read_in_frames;
                ExtAudioFileRead(external_reference, (UInt32*)&frames, &data_buffer);
                m_al_buffer_ready = true;
                m_bytes_of_read += data_size;
                audio_cache::invoking_play_callbacks();
                
                while (!m_exit_read_data_task && m_bytes_of_read + data_size < m_data_size)
                {
                    data_buffer.mBuffers[0].mData = m_pcm_data + m_bytes_of_read;
                    frames = read_in_frames;
                    ExtAudioFileRead(external_reference, (UInt32*)&frames, &data_buffer);
                    m_bytes_of_read += data_size;
                }
                
                data_size = m_data_size - m_bytes_of_read;
                if (!m_exit_read_data_task && data_size > 0)
                {
                    data_buffer.mBuffers[0].mDataByteSize = (UInt32)data_size;
                    data_buffer.mBuffers[0].mData = m_pcm_data + m_bytes_of_read;
                    frames = read_in_frames;
                    ExtAudioFileRead(external_reference, (UInt32*)&frames, &data_buffer);
                }
                
                m_bytes_of_read = m_data_size;
            }
            else
            {
                m_queue_buffer_frames = file_format.mSampleRate * k_queue_buffer_time_step;
                m_queue_buffer_bytes = m_queue_buffer_frames * m_output_format.mBytesPerFrame;
                
                data_buffer.mNumberBuffers = 1;
                data_buffer.mBuffers[0].mNumberChannels = m_output_format.mChannelsPerFrame;
                for (i32 index = 0; index < k_queue_buffer_num; ++index)
                {
                    m_queue_buffers[index] = (char*)malloc(m_queue_buffer_bytes);
                    
                    data_buffer.mBuffers[0].mDataByteSize = m_queue_buffer_bytes;
                    data_buffer.mBuffers[0].mData = m_queue_buffers[index];
                    frames = m_queue_buffer_frames;
                    ExtAudioFileRead(external_reference, (UInt32*)&frames, &data_buffer);
                    
                    m_queue_buffer_size[index] = data_buffer.mBuffers[0].mDataByteSize;
                }
            }
            
        EXIT_THREAD:

            if(external_reference)
            {
                ExtAudioFileDispose(external_reference);
            }
            
            m_read_data_task_mutex.unlock();
            if(m_queue_buffer_frames > 0)
            {
                m_al_buffer_ready = true;
            }
            else
            {
                m_load_fail = true;
            }
            
            audio_cache::invoking_play_callbacks();
            audio_cache::invoking_load_callbacks();
        }
        
        void audio_cache::add_play_callback(const std::function<void()>& callback)
        {
            m_callback_mutex.lock();
            if(m_al_buffer_ready)
            {
                callback();
            }
            else if(!m_load_fail)
            {
                m_callbacks.push_back(callback);
            }
            m_callback_mutex.unlock();
        }
        
        void audio_cache::invoking_play_callbacks()
        {
            m_callback_mutex.lock();
            auto count = m_callbacks.size();
            for (size_t index = 0; index < count; ++index)
            {
                m_callbacks[index]();
            }
            m_callbacks.clear();
            m_callback_mutex.unlock();
        }
        
        void audio_cache::add_load_callback(const std::function<void(bool)>& callback)
        {
            if (m_al_buffer_ready)
            {
                callback(true);
            }
            else if(m_load_fail)
            {
                callback(false);
            }
            else
            {
                m_load_callbacks.push_back(callback);
            }
        }
        
        void audio_cache::invoking_load_callbacks()
        {
            gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
            operation->set_execution_callback([=]() {
                auto count = m_load_callbacks.size();
                for (size_t index = 0; index < count; ++index)
                {
                    m_load_callbacks[index](m_al_buffer_ready);
                }
                m_load_callbacks.clear();
            });
            operation->add_to_execution_queue();
        }
    }
}

