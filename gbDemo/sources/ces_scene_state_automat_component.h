//
//  ces_character_state_automat_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/21/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_scene_state_automat_component : public gb::ces_base_component
    {
    public:
        
        enum e_mode
        {
            e_mode_none = 0,
            e_mode_main_menu,
            e_mode_in_game
        };
        
        enum e_state
        {
            e_state_none = 0,
            e_state_should_preload,
            e_state_pre_loading,
            e_state_loading
        };
        
        static const f32 k_loading_progress_inc_value;
        
    private:
        
        e_mode m_mode = e_mode_none;
        e_state m_state = e_state_none;
        
        f32 m_loading_progress = 0.f;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_scene_state_automat_component, gb::ces_base_component::g_guids_container)
        ces_scene_state_automat_component();
        ~ces_scene_state_automat_component() = default;
        
        std::property_rw<e_mode> mode;
        std::property_rw<e_state> state;
        std::property_rw<f32> loading_progress;
    };
};
