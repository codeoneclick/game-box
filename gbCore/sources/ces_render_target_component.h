//
//  ces_render_target_component.h
//  gbCore
//
//  Created by serhii serhiiv on 8/29/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#pragma once

#include "ces_base_component.h"

namespace gb
{
    class ces_render_target_component : public ces_base_component
    {
    public:
        
        typedef std::function<void(const ces_entity_shared_ptr& entity)> on_render_begin_callback_t;
        typedef std::function<void(const ces_entity_shared_ptr& entity)> on_render_end_callback_t;
        
    private:
        
        render_target_shared_ptr m_render_target;
        material_shared_ptr m_material;
        on_render_begin_callback_t m_render_begin_callback;
        on_render_end_callback_t m_render_end_callback;
        
    protected:
        
    public:
        
        STTI_CLASS_GUID(ces_render_target_component, ces_base_component::g_guids_container)
        ces_render_target_component();
        ~ces_render_target_component();
        
        void setup(const render_target_shared_ptr& render_target,
                   const material_shared_ptr& material);
        
        render_target_shared_ptr get_render_target() const;
        material_shared_ptr get_material() const;
        
        void set_on_render_begin_callback(const on_render_begin_callback_t& callback);
        void set_on_render_end_callback(const on_render_end_callback_t& callback);
        
        on_render_begin_callback_t get_on_render_begin_callback() const;
        on_render_end_callback_t get_on_render_end_callback() const;
    };
};

