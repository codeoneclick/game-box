//
//  ces_hit_bounds_component.cpp
//  gbDemo
//
//  Created by serhii serhiiv on 2/3/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "ces_hit_bounds_component.h"

namespace game
{
    ces_hit_bounds_component::ces_hit_bounds_component() :
    m_on_hit_callback(nullptr)
    {

    }
    
    ces_hit_bounds_component::~ces_hit_bounds_component()
    {
        
    }
    
    bool ces_hit_bounds_component::is_hit_callback_exist() const
    {
        return m_on_hit_callback != nullptr;
    }
    
    void ces_hit_bounds_component::set_hit_callback(const on_hit_callback_t& callback)
    {
        m_on_hit_callback = callback;
    }
    
    ces_hit_bounds_component::on_hit_callback_t ces_hit_bounds_component::get_hit_callback() const
    {
        return m_on_hit_callback;
    }
    
    void ces_hit_bounds_component::set_executor(const gb::ces_entity_shared_ptr& entity)
    {
        m_executor = entity;
    }
    
    gb::ces_entity_shared_ptr ces_hit_bounds_component::get_executor() const
    {
        gb::ces_entity_shared_ptr entity = nullptr;
        if(!m_executor.expired())
        {
            entity = m_executor.lock();
        }
        return entity;
    }
}
