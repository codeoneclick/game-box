//
//  ces_sound_system.cpp
//  gb_sound
//
//  Created by serhii serhiiv on 6/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_sound_system.h"
#include "ces_sound_component.h"
#include "audio_engine.h"
#include "common.h"

namespace gb
{
    namespace al
    {
        ces_sound_system::ces_sound_system()
        {
            ces_base_system::add_required_component_guid(m_sound_components_mask, ces_sound_component::class_guid());
            ces_base_system::add_required_components_mask(m_sound_components_mask);
        }
        
        ces_sound_system::~ces_sound_system()
        {
            
        }
        
        void ces_sound_system::on_feed_start(f32 deltatime)
        {
            
        }
        
        void ces_sound_system::on_feed(const ces_entity_shared_ptr& root, f32 dt)
        {
            ces_base_system::enumerate_entities_with_components(m_sound_components_mask, [=](const gb::ces_entity_shared_ptr& entity) {
                auto sound_component = entity->get_component<ces_sound_component>();
                
                const auto& sounds = sound_component->get_sounds();
                for(const auto& sound : sounds)
                {
                    if (sound.second->m_should_play)
                    {
                        std::string filepath = bundlepath().append(sound.second->m_filename).c_str();
                        sound.second->m_id = audio_engine::play2d(filepath, sound.second->m_is_looped);
                        sound.second->m_should_play = false;
                    }
                    if (sound.second->m_should_stop && sound.second->m_id != -1)
                    {
                        audio_engine::stop(sound.second->m_id);
                        sound.second->m_should_stop = false;
                        sound.second->m_id = -1;
                    }
                    if (sound.second->m_is_volume_changed && sound.second->m_id != -1)
                    {
                        audio_engine::set_volume(sound.second->m_id, sound.second->m_volume);
                        sound.second->m_is_volume_changed = false;
                    }
                    if (sound.second->m_is_pitch_changed && sound.second->m_id != -1)
                    {
                        audio_engine::set_pitch(sound.second->m_id, sound.second->m_pitch);
                        sound.second->m_is_pitch_changed = false;
                    }
                }
            });
        }
        
        void ces_sound_system::on_feed_end(f32 deltatime)
        {
            
        }
    }
}
