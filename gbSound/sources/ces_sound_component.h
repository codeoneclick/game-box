//
//  ces_sound_component.h
//  gb_sound
//
//  Created by serhii serhiiv on 6/2/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    namespace al
    {
        class ces_sound_component : public ces_base_component
        {
        public:
            
            struct sound_data
            {
                std::string m_filename = "";
                i32 m_id = -1;
                bool m_is_need_to_stop = true;
                bool m_is_looped = false;
                bool m_is_triggered = false;
                bool m_is_volume_changed = false;
                bool m_is_pitch_changed = false;
                f32 m_volume = 1.f;
                f32 m_pitch = 1.f;
            };
            
        private:
            
        protected:
            
            std::unordered_map<std::string, std::shared_ptr<sound_data>> m_sounds;
            
        public:
            
            STTI_CLASS_GUID(ces_sound_component, ces_base_component::g_guids_container)
            ces_sound_component();
            ~ces_sound_component();
            
            void add_sound(const std::string& filename,
                           bool is_looped);
            void set_sound_id(const std::string& filename, i32 id);
            void trigger_sound(const std::string& filename, bool is_need_to_stop = true);
            void set_volume(const std::string& filename, f32 volume);
            f32 get_volume(const std::string& filename);
            void set_pitch(const std::string& filename, f32 pitch);
            f32 get_pitch(const std::string& filename);
            
            const std::unordered_map<std::string, std::shared_ptr<sound_data>>& get_sounds() const;
        };
    };
};

