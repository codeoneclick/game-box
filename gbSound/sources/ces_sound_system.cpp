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
            
        }
        
        ces_sound_system::~ces_sound_system()
        {
            
        }
        
        void ces_sound_system::on_feed_start(f32 deltatime)
        {
            
        }
        
        void ces_sound_system::on_feed(const ces_entity_shared_ptr& entity, f32 deltatime)
        {
            ces_sound_system::update_recursively(entity, deltatime);
        }
        
        void ces_sound_system::on_feed_end(f32 deltatime)
        {
            
        }
        
        void ces_sound_system::update_recursively(const ces_entity_shared_ptr& entity, f32 deltatime)
        {
            auto sound_component = entity->get_component<ces_sound_component>();
            
            if(sound_component)
            {
                const auto& sounds = sound_component->get_sounds();
                for(const auto& sound : sounds)
                {
                    if(sound.second->m_is_triggered)
                    {
                        std::string filepath = bundlepath().append(sound.second->m_filename).c_str();
                        sound.second->m_id = audio_engine::play2d(filepath, sound.second->m_is_looped);
                        sound.second->m_is_triggered = false;
                    }
                }
            }
            
            std::vector<ces_entity_shared_ptr> children = entity->children;
            for(const auto& child : children)
            {
                ces_sound_system::update_recursively(child, deltatime);
            }
        }
    }
}
