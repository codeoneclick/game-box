
#include "audio_engine-inl.h"
#include "audio_engine.h"
#include "thread_operation.h"
#include <OpenAL/alc.h>
#include <AVFoundation/AVFoundation.h>

static ALCdevice *s_al_device = nullptr;
static ALCcontext *s_al_context = nullptr;

namespace gb
{
    namespace al
    {
        audio_engine_impl::audio_engine_impl() :
        m_init_loop(true),
        m_current_audio_id(0)
        {
            
        }
        
        audio_engine_impl::~audio_engine_impl()
        {
            if (s_al_context)
            {
                alDeleteSources(k_max_audioinstances, m_al_sources);
                m_audio_caches.clear();
                alcMakeContextCurrent(nullptr);
                alcDestroyContext(s_al_context);
            }
            
            if (s_al_device)
            {
                alcCloseDevice(s_al_device);
            }
        }
        
        bool audio_engine_impl::init()
        {
            bool result = false;
            do
            {
                s_al_device = alcOpenDevice(nullptr);
                
                if (s_al_device)
                {
                    s_al_context = alcCreateContext(s_al_device, nullptr);
                    alcMakeContextCurrent(s_al_context);
                    
                    alGenSources(k_max_audioinstances, m_al_sources);
                    auto al_error = alGetError();
                    if(al_error != AL_NO_ERROR)
                    {
                        break;
                    }
                    
                    for (i32 i = 0; i < k_max_audioinstances; ++i)
                    {
                        m_al_source_used[m_al_sources[i]] = false;
                    }
                    result = true;
                }
            }
            while(false);
            return result;
        }
        
        audio_cache_shared_ptr audio_engine_impl::preload(const std::string& filepath, std::function<void(bool)> callback)
        {
            audio_cache_shared_ptr audio_cache = nullptr;
            
            auto it = m_audio_caches.find(filepath);
            if(it == m_audio_caches.end())
            {
                audio_cache = std::make_shared<gb::al::audio_cache>();
                m_audio_caches[filepath] = audio_cache;
                audio_cache->m_filepath = filepath;
                audio_engine::add_task(std::bind(&audio_cache::read_data_task, audio_cache));
            }
            else
            {
                audio_cache = it->second;
            }
            
            if(audio_cache && callback)
            {
                audio_cache->add_load_callback(callback);
            }
            return audio_cache;
        }
        
        int audio_engine_impl::play2d(const std::string &filepath, bool loop, f32 volume)
        {
            if (s_al_device == nullptr)
            {
                return audio_engine::k_invalid_audio_id;
            }
            
            bool source_flag = false;
            ALuint al_source = 0;
            for(i32 i = 0; i < k_max_audioinstances; ++i)
            {
                al_source = m_al_sources[i];
                if(!m_al_source_used[al_source])
                {
                    source_flag = true;
                    break;
                }
            }
            
            if(!source_flag)
            {
                return audio_engine::k_invalid_audio_id;
            }
            
            auto player = std::make_shared<audio_player>();
            player->m_al_source = al_source;
            player->m_loop = loop;
            player->m_volume = volume;
            
            auto audio_cache = audio_engine_impl::preload(filepath, nullptr);
            if(audio_cache == nullptr)
            {
                return audio_engine::k_invalid_audio_id;
            }
            
            m_thread_mutex.lock();
            m_audio_players[m_current_audio_id] = player;
            m_thread_mutex.unlock();
            
            audio_cache->add_play_callback(std::bind(&audio_engine_impl::play2d_impl, this, audio_cache, m_current_audio_id));
            m_al_source_used[al_source] = true;
            
            if(m_init_loop)
            {
                m_init_loop = false;
                gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
                operation->set_execution_callback([=]() {
                    audio_engine_impl::update(.05f);
                });
                operation->add_to_execution_queue();
            }
            
            return m_current_audio_id++;
        }
        
        void audio_engine_impl::play2d_impl(const audio_cache_shared_ptr& cache, i32 audio_id)
        {
            if(cache->m_al_buffer_ready)
            {
                m_thread_mutex.lock();
                auto it = m_audio_players.find(audio_id);
                if(it != m_audio_players.end() && it->second->play2d(cache))
                {
                    gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
                    operation->set_execution_callback([=, &audio_id]() {
                        if(audio_engine::m_audio_id_infos.find(audio_id) != audio_engine::m_audio_id_infos.end())
                        {
                            audio_engine::m_audio_id_infos[audio_id]->m_state = audio_engine::e_audio_state::e_audio_state_playing;
                        }
                    });
                    operation->add_to_execution_queue();
                }
                m_thread_mutex.unlock();
            }
        }
        
        void audio_engine_impl::set_volume(i32 audio_id, f32 volume)
        {
            auto player = m_audio_players[audio_id];
            player->m_volume = volume;
            
            if (player->m_ready)
            {
                alSourcef(m_audio_players[audio_id]->m_al_source, AL_GAIN, volume);
            }
        }
        
        void audio_engine_impl::set_loop(i32 audio_id, bool loop)
        {
            auto player = m_audio_players[audio_id];
            if(player->m_ready)
            {
                if(player->m_streaming_source)
                {
                    player->set_loop(loop);
                }
                else
                {
                    if(loop)
                    {
                        alSourcei(player->m_al_source, AL_LOOPING, AL_TRUE);
                    }
                    else
                    {
                        alSourcei(player->m_al_source, AL_LOOPING, AL_FALSE);
                    }
                }
            }
            else
            {
                player->m_loop = loop;
            }
        }
        
        bool audio_engine_impl::pause(i32 audio_id)
        {
            bool result = true;
            alSourcePause(m_audio_players[audio_id]->m_al_source);
            
            auto error = alGetError();
            if(error != AL_NO_ERROR)
            {
                result = false;
            }
            return result;
        }
        
        bool audio_engine_impl::resume(i32 audio_id)
        {
            bool result = true;
            alSourcePlay(m_audio_players[audio_id]->m_al_source);
            
            auto error = alGetError();
            if(error != AL_NO_ERROR)
            {
                result = false;
            }
            return result;
        }
        
        void audio_engine_impl::stop(i32 audio_id)
        {
            auto player = m_audio_players[audio_id];
            player->destroy();
            m_al_source_used[player->m_al_source] = false;
        }
        
        void audio_engine_impl::stop_all()
        {
            for(auto& player : m_audio_players)
            {
                player.second->destroy();
            }
            for(i32 index = 0; index < k_max_audioinstances; ++index)
            {
                m_al_source_used[m_al_sources[index]] = false;
            }
        }
        
        float audio_engine_impl::get_duration(i32 audio_id)
        {
            auto player = m_audio_players[audio_id];
            if(player->m_ready)
            {
                return player->m_audio_cache->m_duration;
            }
            else
            {
                return audio_engine::k_time_unknown;
            }
        }
        
        f32 audio_engine_impl::get_current_time(i32 audio_id)
        {
            f32 result = .0f;
            auto player = m_audio_players[audio_id];
            if(player->m_ready)
            {
                if(player->m_streaming_source)
                {
                    result = player->get_time();
                }
                else
                {
                    alGetSourcef(player->m_al_source, AL_SEC_OFFSET, &result);
                    
                    auto error = alGetError();
                    if(error != AL_NO_ERROR)
                    {
                        assert(false);
                    }
                }
            }
            return result;
        }
        
        bool audio_engine_impl::set_current_time(i32 audio_id, f32 time)
        {
            bool result = false;
            auto player = m_audio_players[audio_id];
            
            do
            {
                if(!player->m_ready)
                {
                    break;
                }
                
                if(player->m_streaming_source)
                {
                    result = player->set_time(time);
                    break;
                }
                else
                {
                    if(player->m_audio_cache->m_bytes_of_read != player->m_audio_cache->m_data_size &&
                       (time * player->m_audio_cache->m_sample_rate * player->m_audio_cache->m_bytes_per_frame) > player->m_audio_cache->m_bytes_of_read)
                    {
                        assert(false);
                        break;
                    }
                    
                    alSourcef(player->m_al_source, AL_SEC_OFFSET, time);
                    
                    auto error = alGetError();
                    if(error != AL_NO_ERROR)
                    {
                        assert(false);
                    }
                    result = true;
                }
            } while (0);
            
            return result;
        }
        
        void audio_engine_impl::set_finish_callback(i32 audio_id, const std::function<void (i32, const std::string &)> &callback)
        {
            m_audio_players[audio_id]->m_finish_callbak = callback;
        }
        
        void audio_engine_impl::update(f32 dt)
        {
            ALint source_state;
            i32 audio_id;
            audio_player_shared_ptr player;
            
            for (auto it = m_audio_players.begin(); it != m_audio_players.end();)
            {
                audio_id = it->first;
                player = it->second;
                alGetSourcei(player->m_al_source, AL_SOURCE_STATE, &source_state);
                
                if(player->m_remove_by_audio_engine)
                {
                    audio_engine::remove(audio_id);
                    m_thread_mutex.lock();
                    it = m_audio_players.erase(it);
                    m_thread_mutex.unlock();
                }
                else if(player->m_ready && source_state == AL_STOPPED)
                {
                    m_al_source_used[player->m_al_source] = false;
                    if (player->m_finish_callbak)
                    {
                        auto& audio_info = audio_engine::m_audio_id_infos[audio_id];
                        player->m_finish_callbak(audio_id, *(audio_info->m_filepath));
                    }
                    
                    audio_engine::remove(audio_id);
                    m_thread_mutex.lock();
                    it = m_audio_players.erase(it);
                    m_thread_mutex.unlock();
                }
                else
                {
                    ++it;
                }
            }
            if(m_audio_players.empty())
            {
                m_init_loop = true;
            }
            else
            {
                gb::thread_operation_shared_ptr operation = std::make_shared<gb::thread_operation>(gb::thread_operation::e_thread_operation_queue_main);
                operation->set_execution_callback([=, &audio_id]() {
                     audio_engine_impl::update(.05f);
                });
                operation->add_to_execution_queue();
            }
        }
        
        void audio_engine_impl::uncache(const std::string &filepath)
        {
            m_audio_caches.erase(filepath);
        }
        
        void audio_engine_impl::uncache_all()
        {
            m_audio_caches.clear();
        }
    }
}
