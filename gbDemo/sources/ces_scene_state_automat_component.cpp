//
//  ces_character_state_automat_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 7/21/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_scene_state_automat_component.h"

namespace game
{
    const f32 ces_scene_state_automat_component::k_loading_progress_inc_value = .05f;
    
    ces_scene_state_automat_component::ces_scene_state_automat_component()
    {
        mode.getter([=]() {
            return m_mode;
        });
        
        mode.setter([=](e_mode value) {
            m_from_mode = m_mode;
            m_mode = value;
        });
        
        from_mode.getter([=]() {
            return m_from_mode;
        });
        
        state.getter([=]() {
            return m_state;
        });
        
        state.setter([=](e_state value) {
            m_state = value;
        });
        
        loading_progress.getter([=]() {
            return m_loading_progress;
        });
        
        loading_progress.setter([=](f32 value) {
            m_loading_progress = value;
        });
    }
}
