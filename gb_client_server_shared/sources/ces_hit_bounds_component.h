//
//  ces_hit_bounds_component.h
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace game
{
    class ces_hit_bounds_component : public gb::ces_base_component
    {
    public:
        
        typedef std::function<void(const gb::ces_entity_shared_ptr&)> on_hit_callback_t;
        
    private:
        
        on_hit_callback_t m_on_hit_callback;
        gb::ces_entity_weak_ptr m_executor;
        
    protected:
        
    public:
        
        CTTI_CLASS_GUID(ces_hit_bounds_component, gb::ces_base_component::g_guids_container)
        ces_hit_bounds_component();
        ~ces_hit_bounds_component();
        
        bool is_hit_callback_exist() const;
        void set_hit_callback(const on_hit_callback_t& callback);
        on_hit_callback_t get_hit_callback() const;
        
        void set_executor(const gb::ces_entity_shared_ptr& entity);
        gb::ces_entity_shared_ptr get_executor() const;
    };
};

