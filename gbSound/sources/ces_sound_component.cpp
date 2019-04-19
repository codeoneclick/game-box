//
//  ces_sound_component.cpp
//  gb_sound
//
//  Created by serhii serhiiv on 6/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_sound_component.h"
#include "audio_engine.h"

namespace gb
{
    namespace al
    {
        ces_sound_component::ces_sound_component()
        {
            
        }
        
        ces_sound_component::~ces_sound_component()
        {
            for(const auto& sound : m_sounds)
            {
                if(sound.second->m_id != -1 && sound.second->m_is_need_to_stop)
                {
                    audio_engine::stop(sound.second->m_id);
                }
            }
        }
        
        void ces_sound_component::add_sound(const std::string& filename,
                                            bool is_looped)
        {
            auto sound_data = std::make_shared<ces_sound_component::sound_data>();
            sound_data->m_filename = filename;
            sound_data->m_is_looped = is_looped;
            m_sounds.insert(std::make_pair(filename, sound_data));
        }
        
        void ces_sound_component::set_sound_id(const std::string& filename, i32 id)
        {
            auto it = m_sounds.find(filename);
            if(it != m_sounds.end())
            {
                it->second->m_id = id;
            }
        }
        
        void ces_sound_component::trigger_sound(const std::string& filename, bool is_need_to_stop)
        {
            auto it = m_sounds.find(filename);
            if(it != m_sounds.end())
            {
                it->second->m_is_triggered = true;
                it->second->m_is_need_to_stop = is_need_to_stop;
            }
        }
        
        void ces_sound_component::set_volume(const std::string& filename, f32 volume)
        {
            auto it = m_sounds.find(filename);
            if(it != m_sounds.end())
            {
                it->second->m_volume = volume;
                it->second->m_is_volume_changed = true;
            }
        }
        
        f32 ces_sound_component::get_volume(const std::string& filename)
        {
            f32 result = 0.f;
            auto it = m_sounds.find(filename);
            if(it != m_sounds.end())
            {
                result = it->second->m_volume;
            }
            return result;
        }
        
        void ces_sound_component::set_pitch(const std::string& filename, f32 pitch)
        {
            auto it = m_sounds.find(filename);
            if(it != m_sounds.end())
            {
                it->second->m_pitch = pitch;
                it->second->m_is_pitch_changed = true;
            }
        }
        
        f32 ces_sound_component::get_pitch(const std::string& filename)
        {
            f32 result = 1.f;
            auto it = m_sounds.find(filename);
            if(it != m_sounds.end())
            {
                result = it->second->m_pitch;
            }
            return result;
        }
        
        const std::unordered_map<std::string, std::shared_ptr<ces_sound_component::sound_data>>& ces_sound_component::get_sounds() const
        {
            return m_sounds;
        }
    }
}

