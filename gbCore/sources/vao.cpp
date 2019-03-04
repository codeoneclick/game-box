//
//  vao.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "vao.h"
#include "vbo.h"
#include "ibo.h"

namespace gb
{
    std::queue<ui32> vao::m_handlers_graveyard;
    std::mutex vao::m_graveyard_mutex;
    
    vao::vao(bool is_using_batch) :
    m_is_using_batch(is_using_batch)
    {
        if(!m_is_using_batch)
        {
            gl::command::create_vertex_arrays(1, &m_handle);
            gl::command::bind_vertex_array(m_handle);
        }
    }
    
    vao::~vao()
    {
        if(!m_is_using_batch)
        {
            vao::add_to_graveyard(m_handle);
        }
    }
    
    void vao::add_to_graveyard(ui32 handler)
    {
        std::lock_guard<std::mutex> guard(m_graveyard_mutex);
        m_handlers_graveyard.push(handler);
    }
    
    void vao::bind(const std::shared_ptr<vao>& state)
    {
        if(!state->m_is_using_batch)
        {
            gl::command::bind_vertex_array(state->m_handle);
        }
    }
    
    void vao::unbind()
    {
        gl::command::bind_vertex_array(NULL);
    }
}
