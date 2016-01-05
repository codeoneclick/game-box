//
//  ces_batch_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 10/28/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ces_batch_component.h"

namespace gb
{
    ces_batch_component::ces_batch_component(const std::string& guid) :
    m_batch(nullptr),
    m_guid(guid)
    {
        m_type = e_ces_component_type_batch;
    }
    
    ces_batch_component::~ces_batch_component()
    {
        
    }
    
    void ces_batch_component::set_batch(const batch_shared_ptr& batch)
    {
        m_batch = batch;
    }
    
    batch_shared_ptr ces_batch_component::get_batch() const
    {
        return m_batch;
    }
    
    batch::e_batch_render_state ces_batch_component::get_render_state(const std::string& guid) const
    {
        assert(m_batch);
        return m_batch->get_render_state(guid);
    }
    
    void ces_batch_component::set_render_state(const std::string& guid, batch::e_batch_render_state state)
    {
        assert(m_batch);
        m_batch->set_render_state(guid, state);
    }
    
    const std::string& ces_batch_component::get_guid() const
    {
        return m_guid;
    }
    
    void ces_batch_component::cleanup()
    {

    }
}