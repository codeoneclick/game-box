//
//  ces_render_target_component.cpp
//  gbCore
//
//  Created by serhii serhiiv on 8/29/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_render_target_component.h"

namespace gb
{
    ces_render_target_component::ces_render_target_component() :
    m_render_target(nullptr),
    m_material(nullptr),
    m_render_begin_callback(nullptr),
    m_render_end_callback(nullptr)
    {
        
    }
    
    ces_render_target_component::~ces_render_target_component()
    {
        
    }
    
    void ces_render_target_component::setup(const render_target_shared_ptr& render_target,
                                            const material_shared_ptr& material)
    {
        m_render_target = render_target;
        m_material = material;
    }
    
    render_target_shared_ptr ces_render_target_component::get_render_target() const
    {
        return m_render_target;
    }
    
    material_shared_ptr ces_render_target_component::get_material() const
    {
        return m_material;
    }
    
    void ces_render_target_component::set_on_render_begin_callback(const on_render_begin_callback_t& callback)
    {
        m_render_begin_callback = callback;
    }
    
    void ces_render_target_component::set_on_render_end_callback(const on_render_end_callback_t& callback)
    {
        m_render_end_callback = callback;
    }
    
    ces_render_target_component::on_render_begin_callback_t ces_render_target_component::get_on_render_begin_callback() const
    {
        return m_render_begin_callback;
    }
    
    ces_render_target_component::on_render_end_callback_t ces_render_target_component::get_on_render_end_callback() const
    {
        return m_render_end_callback;
    }
}
