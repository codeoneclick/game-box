
#include "audio_engine.h"
#include "common.h"

#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)

#include "apple/audio_engine-inl.h"

#endif

namespace gb
{
    namespace al
    {

#if defined(__WINOS__)

#define k_max_audioinstances 0

#endif
        
#define k_time_delay_precision 0.0001
        
        const i32 audio_engine::k_invalid_audio_id = -1;
        const f32 audio_engine::k_time_unknown = -1.f;
        
        std::unordered_map<std::string,std::list<i32>> audio_engine::m_audio_path_ids;

        std::unordered_map<std::string, std::shared_ptr<audio_engine::profile_helper>> audio_engine::m_audio_path_profile_helpers;

        ui32 audio_engine::m_max_instances = k_max_audioinstances;
        
        std::shared_ptr<audio_engine::profile_helper> audio_engine::m_default_profile_helper = nullptr;
        
        std::unordered_map<i32, std::shared_ptr<audio_engine::audio_info>> audio_engine::m_audio_id_infos;
        
        audio_engine_impl_shared_ptr audio_engine::m_audio_engine_impl = nullptr;
        
        std::shared_ptr<audio_engine::audio_engine_thread_pool> audio_engine::m_thread_pool = nullptr;
        
        class audio_engine::audio_engine_thread_pool
        {
        private:
            
            void update()
            {
                while (true)
                {
                    std::function<void()> current_task = nullptr;
                    {
                        std::unique_lock<std::mutex> lock(m_queue_mutex);
                        if (m_stop)
                        {
                            break;
                        }
                        if (!m_task_queue.empty())
                        {
                            current_task = std::move(m_task_queue.front());
                            m_task_queue.pop();
                        }
                        else
                        {
                            m_task_condition.wait(lock);
                            continue;
                        }
                    }
                    current_task();
                }
            }
            
            std::vector<std::thread>  m_workers;
            std::queue<std::function<void()> > m_task_queue;
            
            std::mutex m_queue_mutex;
            std::condition_variable m_task_condition;
            bool m_detach;
            bool m_stop;
            
        protected:
            
            
        public:
            
            audio_engine_thread_pool(bool detach, i32 threads = 4) :
            m_detach(detach),
            m_stop(false)
            {
                for (i32 index = 0; index < threads; ++index)
                {
                    m_workers.emplace_back(std::thread(std::bind(&audio_engine_thread_pool::update, this)));
                    if (m_detach)
                    {
                        m_workers[index].detach();
                    }
                }
            }
            
            void add_task(const std::function<void()> &task)
            {
                std::unique_lock<std::mutex> lock(m_queue_mutex);
                m_task_queue.emplace(task);
                m_task_condition.notify_one();
            }
            
            ~audio_engine_thread_pool()
            {
                {
                    std::unique_lock<std::mutex> lock(m_queue_mutex);
                    m_stop = true;
                    m_task_condition.notify_all();
                }
                
                if (!m_detach)
                {
                    for (auto&& worker : m_workers)
                    {
                        worker.join();
                    }
                }
            }
        };
        
        void audio_engine::end()
        {
        }
        
        bool audio_engine::init()
        {
#if !defined(__WINOS__)
            if (m_audio_engine_impl == nullptr)
            {
                m_audio_engine_impl = std::make_shared<audio_engine_impl>();
                if(!m_audio_engine_impl ||  !m_audio_engine_impl->init() )
                {
                    return false;
                }
            }
            
#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)
            
            if (m_audio_engine_impl && m_thread_pool == nullptr)
            {
                m_thread_pool = std::make_shared<audio_engine_thread_pool>(false);
            }
    
#endif
            
            return true;

#else

			return false;

#endif

        }
        
        i32 audio_engine::play2d(const std::string& filepath, bool loop, f32 volume, f32 pitch)
        {

#if !defined(__WINOS__)

            i32 result = audio_engine::k_invalid_audio_id;
            
            do
            {
                if(!init())
                {
                    break;
                }
                
                auto profile_helper = m_default_profile_helper;
                if (m_audio_id_infos.size() >= m_max_instances)
                {
                    break;
                }
                
                if (profile_helper)
                {
                    if(profile_helper->m_profile->m_max_instances != 0 && profile_helper->m_audio_ids.size() >= profile_helper->m_profile->m_max_instances)
                    {
                        break;
                    }
                    if (profile_helper->m_profile->m_min_delay > k_time_delay_precision)
                    {
                        auto current_time = ::get_current_time();
                        if (profile_helper->m_last_play_time > k_time_delay_precision && current_time - profile_helper->m_last_play_time <= profile_helper->m_profile->m_min_delay)
                        {
                            break;
                        }
                    }
                }
                
                if (volume < 0.f)
                {
                    volume = 0.f;
                }
                else if (volume > 1.f)
                {
                    volume = 1.f;
                }
                
                result = m_audio_engine_impl->play2d(filepath, loop, volume, pitch);
                if (result != k_invalid_audio_id)
                {
                    m_audio_path_ids[filepath].push_back(result);
                    auto iterator = m_audio_path_ids.find(filepath);
                    
                    m_audio_id_infos[result] = std::make_shared<audio_info>();
                    auto& audio_reference = m_audio_id_infos[result];
                    audio_reference->m_volume = volume;
                    audio_reference->m_pitch = pitch;
                    audio_reference->m_loop = loop;
                    audio_reference->m_filepath = &iterator->first;
                    
                    if (profile_helper)
                    {
                        profile_helper->m_last_play_time = ::get_current_time();
                        profile_helper->m_audio_ids.push_back(result);
                    }
                    audio_reference->m_profile_helper = profile_helper;
                }
            } while (0);
            
            return result;
#else

			return 0;

#endif

        }
        
        void audio_engine::set_loop(i32 audio_id, bool loop)
        {

#if !defined(__WINOS__)

            auto iterator = m_audio_id_infos.find(audio_id);
            if (iterator != m_audio_id_infos.end() && iterator->second->m_loop != loop)
            {
                m_audio_engine_impl->set_loop(audio_id, loop);
                iterator->second->m_loop = loop;
            }

#endif

        }
        
        void audio_engine::set_volume(i32 audio_id, f32 volume)
        {

#if !defined(__WINOS__)

            auto iterator = m_audio_id_infos.find(audio_id);
            if (iterator != m_audio_id_infos.end())
            {
                if (volume < 0.f)
                {
                    volume = 0.f;
                }
                else if (volume > 1.f)
                {
                    volume = 1.f;
                }
                
                if (iterator->second->m_volume != volume)
                {
                    m_audio_engine_impl->set_volume(audio_id, volume);
                    iterator->second->m_volume = volume;
                }
            }

#endif

        }
        
        void audio_engine::set_pitch(i32 audio_id, f32 pitch)
        {
#if !defined(__WINOS__)
            
            auto iterator = m_audio_id_infos.find(audio_id);
            if (iterator != m_audio_id_infos.end())
            {
                if (pitch < .5f)
                {
                    pitch = .5f;
                }
                else if (pitch > 2.f)
                {
                    pitch = 2.f;
                }
                
                if (iterator->second->m_pitch != pitch)
                {
                    m_audio_engine_impl->set_pitch(audio_id, pitch);
                    iterator->second->m_pitch = pitch;
                }
            }
            
#endif
        }
        
        f32 audio_engine::get_pitch(i32 audio_id)
        {
            auto iterator = m_audio_id_infos.find(audio_id);
            if (iterator != m_audio_id_infos.end())
            {
                return iterator->second->m_pitch;
            }
            return 1.f;
        }
        
        void audio_engine::pause(i32 audio_id)
        {

#if !defined(__WINOS__)

            auto iterator = m_audio_id_infos.find(audio_id);
            if(iterator != m_audio_id_infos.end() && iterator->second->m_state == e_audio_state::e_audio_state_playing)
            {
                m_audio_engine_impl->pause(audio_id);
                iterator->second->m_state = e_audio_state::e_audio_state_paused;
            }

#endif

        }
        
        void audio_engine::pause_all()
        {

#if !defined(__WINOS__)

            auto end = m_audio_id_infos.end();
            for (auto iterator = m_audio_id_infos.begin(); iterator != end; ++iterator)
            {
                if (iterator->second->m_state == e_audio_state::e_audio_state_playing)
                {
                    m_audio_engine_impl->pause(iterator->first);
                    iterator->second->m_state = e_audio_state::e_audio_state_paused;
                }
            }

#endif

        }
        
        void audio_engine::resume(i32 audio_id)
        {

#if !defined(__WINOS__)

            auto iterator = m_audio_id_infos.find(audio_id);
            if (iterator != m_audio_id_infos.end() && iterator->second->m_state == e_audio_state::e_audio_state_paused)
            {
                m_audio_engine_impl->resume(audio_id);
                iterator->second->m_state = e_audio_state::e_audio_state_playing;
            }

#endif

        }
        
        void audio_engine::resume_all()
        {

#if !defined(__WINOS__)

            auto end = m_audio_id_infos.end();
            for (auto iterator = m_audio_id_infos.begin(); iterator != end; ++iterator)
            {
                if (iterator->second->m_state == e_audio_state::e_audio_state_paused)
                {
                    m_audio_engine_impl->resume(iterator->first);
                    iterator->second->m_state = e_audio_state::e_audio_state_playing;
                }
            }

#endif

        }
        
        void audio_engine::stop(i32 audio_id)
        {

#if !defined(__WINOS__)

            auto iterator = m_audio_id_infos.find(audio_id);
            if (iterator != m_audio_id_infos.end())
            {
                m_audio_engine_impl->stop(audio_id);
                remove(audio_id);
            }

#endif

        }
        
        void audio_engine::remove(i32 audio_id)
        {
            auto iterator = m_audio_id_infos.find(audio_id);
            if (iterator != m_audio_id_infos.end())
            {
                if (iterator->second->m_profile_helper)
                {
                    iterator->second->m_profile_helper->m_audio_ids.remove(audio_id);
                }
                m_audio_path_ids[*iterator->second->m_filepath].remove(audio_id);
                m_audio_id_infos.erase(audio_id);
            }
        }
        
        void audio_engine::stop_all()
        {

#if !defined(__WINOS__)

            if(!m_audio_engine_impl)
            {
                return;
            }
            m_audio_engine_impl->stop_all();
            auto end = m_audio_id_infos.end();
            for (auto iterator = m_audio_id_infos.begin(); iterator != end; ++iterator)
            {
                if (iterator->second->m_profile_helper)
                {
                    iterator->second->m_profile_helper->m_audio_ids.remove(iterator->first);
                }
            }
            m_audio_path_ids.clear();
            m_audio_id_infos.clear();

#endif

		}
        
        void audio_engine::uncache(const std::string &filepath)
        {

#if !defined(__WINOS__)

            if(m_audio_path_ids.find(filepath) != m_audio_path_ids.end())
            {
                auto end = m_audio_path_ids[filepath].end();
                for (auto iterator = m_audio_path_ids[filepath].begin() ; iterator != end; ++iterator)
                {
                    auto audio_id = *iterator;
                    m_audio_engine_impl->stop(audio_id);
                    
                    auto iterator_02 = m_audio_id_infos.find(audio_id);
                    if (iterator_02 != m_audio_id_infos.end()){
                        if (iterator_02->second->m_profile_helper)
                        {
                            iterator_02->second->m_profile_helper->m_audio_ids.remove(audio_id);
                        }
                        m_audio_id_infos.erase(audio_id);
                    }
                }
                m_audio_path_ids.erase(filepath);
            }
            
            if (m_audio_engine_impl)
            {
                m_audio_engine_impl->uncache(filepath);
            }

#endif

        }
        
        void audio_engine::uncache_all()
        {

#if !defined(__WINOS__)

            if(!m_audio_engine_impl)
            {
                return;
            }
            audio_engine::stop_all();
            m_audio_engine_impl->uncache_all();

#endif

        }
        
        f32 audio_engine::get_duration(i32 audio_id)
        {

#if !defined(__WINOS__)

            auto iterator = m_audio_id_infos.find(audio_id);
            if(iterator != m_audio_id_infos.end() && iterator->second->m_state != e_audio_state::e_audio_state_initializing)
            {
                if(iterator->second->m_duration == k_time_unknown)
                {
                    iterator->second->m_duration = m_audio_engine_impl->get_duration(audio_id);
                }
                return iterator->second->m_duration;
            }

#endif

            return k_time_unknown;
        }
        
        bool audio_engine::set_current_time(i32 audio_id, f32 time)
        {

#if !defined(__WINOS__)

            auto iterator = m_audio_id_infos.find(audio_id);
            if(iterator != m_audio_id_infos.end() && iterator->second->m_state != e_audio_state::e_audio_state_initializing)
            {
                return m_audio_engine_impl->set_current_time(audio_id, time);
            }

#endif

            return false;
        }
        
        float audio_engine::get_current_time(i32 audio_id)
        {

#if !defined(__WINOS__)

            auto iterator = m_audio_id_infos.find(audio_id);
            if(iterator != m_audio_id_infos.end() && iterator->second->m_state != e_audio_state::e_audio_state_initializing)
            {
                return m_audio_engine_impl->get_current_time(audio_id);
            }

#endif

            return 0.f;
        }
        
        void audio_engine::set_finish_callback(i32 audio_id, const std::function<void(i32, const std::string &)> &callback)
        {

#if !defined(__WINOS__)

            auto iterator = m_audio_id_infos.find(audio_id);
            if(iterator != m_audio_id_infos.end())
            {
                m_audio_engine_impl->set_finish_callback(audio_id, callback);
            }

#endif

        }
        
        bool audio_engine::set_max_audio_instance(i32 max_instances)
        {
            if(max_instances > 0 && max_instances <= k_max_audioinstances)
            {
                m_max_instances = max_instances;
                return true;
            }
            return false;
        }
        
        bool audio_engine::is_loop(i32 audio_id)
        {
            auto iterator = m_audio_id_infos.find(audio_id);
            if(iterator != m_audio_id_infos.end())
            {
                return iterator->second->m_loop;
            }
            return false;
        }
        
        f32 audio_engine::get_volume(i32 audio_id)
        {
            auto iterator = m_audio_id_infos.find(audio_id);
            if (iterator != m_audio_id_infos.end())
            {
                return iterator->second->m_volume;
            }
            return 0.f;
        }
        
        audio_engine::e_audio_state audio_engine::get_state(i32 audio_id)
        {
            auto iterator = m_audio_id_infos.find(audio_id);
            if(iterator != m_audio_id_infos.end())
            {
                return iterator->second->m_state;
            }
            return audio_engine::e_audio_state::e_audio_state_error;
        }
        
        std::shared_ptr<audio_profile> audio_engine::get_profile(i32 audio_id)
        {
            auto iterator = m_audio_id_infos.find(audio_id);
            if(iterator != m_audio_id_infos.end())
            {
                return iterator->second->m_profile_helper->m_profile;
            }
            return nullptr;
        }
        
        std::shared_ptr<audio_profile> audio_engine::get_default_profile()
        {
            if(m_default_profile_helper == nullptr)
            {
                m_default_profile_helper = std::make_shared<profile_helper>();
            }
            
            return m_default_profile_helper->m_profile;
        }
        
        std::shared_ptr<audio_profile> audio_engine::get_profile(const std::string &name)
        {
            auto iterator = m_audio_path_profile_helpers.find(name);
            if(iterator != m_audio_path_profile_helpers.end())
            {
                return iterator->second->m_profile;
            }
            else
            {
                return nullptr;
            }
        }
        
        void audio_engine::preload(const std::string& filepath, std::function<void(bool is_success)> callback)
        {
            audio_engine::init();
            
#if !defined(__WINOS__)

            if (m_audio_engine_impl)
            {
                m_audio_engine_impl->preload(filepath, callback);
            }

#endif

        }
        
        void audio_engine::add_task(const std::function<void()>& task)
        {
            audio_engine::init();
            
            if (m_audio_engine_impl && m_thread_pool)
            {
                m_thread_pool->add_task(task);
            }
        }
    }
}

