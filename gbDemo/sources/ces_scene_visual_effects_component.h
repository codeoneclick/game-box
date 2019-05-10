//
//  ces_scene_visual_effects_component.h
//  gbDemo
//
//  Created by serhii.s on 5/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_scene_visual_effects_component : public gb::ces_base_component
    {
    private:
    
        bool m_is_crossfade_enabled = false;
        bool m_is_noises_enabled = false;
        
        f32 m_crossfade_progress = 0.f;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_scene_visual_effects_component, gb::ces_base_component::g_guids_container)
        ces_scene_visual_effects_component();
        ~ces_scene_visual_effects_component() = default;
        
        std::property_rw<bool> is_crossfade_enabled;
        std::property_rw<bool> is_noises_enabled;
        
        std::property_rw<f32> crossfade_progress;
    };
};

