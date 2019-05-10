//
//  ces_scene_visual_effects_component.cpp
//  gbDemo
//
//  Created by serhii.s on 5/8/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "ces_scene_visual_effects_component.h"

namespace game
{
    ces_scene_visual_effects_component::ces_scene_visual_effects_component()
    {
        is_noises_enabled.getter([=]() {
            return m_is_noises_enabled;
        });
        
        is_noises_enabled.setter([=](bool value) {
            m_is_noises_enabled = value;
        });
        
        is_crossfade_enabled.getter([=]() {
            return m_is_crossfade_enabled;
        });
        
        is_crossfade_enabled.setter([=](bool value) {
            m_is_crossfade_enabled = value;
        });
        
        crossfade_progress.getter([=]() {
            return m_crossfade_progress;
        });
        
        crossfade_progress.setter([=](f32 value) {
            m_crossfade_progress = value;
        });
    }
}
