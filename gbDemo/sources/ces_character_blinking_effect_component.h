//
//  ces_character_blinking_effect_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 7/28/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"
#include "ns_declarations.h"

namespace game
{
    class ces_character_blinking_effect_component : public gb::ces_base_component
    {
    public:
        
        static const i32 k_blinking_count;
        static const f32 k_blinking_timeinterval;
        
         typedef std::function<void(const gb::ces_entity_shared_ptr&)> on_blinking_effect_ended_callback_t;
        
    private:
        
        i32 m_blinking_count;
        f32 m_blinking_timeinterval;
        on_blinking_effect_ended_callback_t m_blinking_effect_ended_callback;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_character_blinking_effect_component, gb::ces_base_component::g_guids_container)
        ces_character_blinking_effect_component();
        ~ces_character_blinking_effect_component();
        
        i32 get_blinking_count() const;
        f32 get_blinking_timeinterval() const;
        
        void set_blinking_count(i32 value);
        void set_blinking_timeinterval(f32 value);
        
        void set_blinking_effect_ended_callback(const on_blinking_effect_ended_callback_t& callback);
        on_blinking_effect_ended_callback_t get_blinking_effect_ended_callback() const;
    };
};

