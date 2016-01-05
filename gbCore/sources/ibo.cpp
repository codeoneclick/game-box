//
//  ibo.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ibo.h"

namespace gb
{
    ibo::ibo(ui32 size, GLenum mode, ui16* mmap) :
    m_allocated_size(size),
    m_used_size(0),
    m_mode(mode),
    m_is_mmap(mmap != nullptr)
    {
        m_type = e_resource_transfering_data_type_ib;
        
        assert(m_allocated_size != 0);
        gl_create_buffers(1, &m_handle);
        
        if(mmap == nullptr)
        {
            m_data = new ui16[m_allocated_size];
            memset(m_data, 0x0, sizeof(ui16) * m_allocated_size);
        }
        else
        {
            m_data = mmap;
        }
    }
    
    ibo::~ibo(void)
    {
        gl_delete_buffers(1, &m_handle);
        if(!m_is_mmap)
        {
            delete[] m_data;
        }
    }
    
    ui32 ibo::get_allocated_size(void) const
    {
        return m_allocated_size;
    }
    
    ui32 ibo::get_used_size(void) const
    {
        return m_used_size;
    }
    
    ui16* ibo::lock(void) const
    {
        return m_data;
    }
    
    void ibo::unlock(ui32 size)
    {
        assert(m_data != nullptr);
        assert(m_allocated_size != 0);
        m_used_size = size > 0 && size < m_allocated_size ? size : m_allocated_size;
        gl_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
        gl_push_buffer_data(GL_ELEMENT_ARRAY_BUFFER, sizeof(ui16) * m_used_size, m_data, m_mode);
    }
    
    void ibo::bind(void) const
    {
        if(m_used_size != 0)
        {
            gl_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
        }
    }
    
    void ibo::unbind(void) const
    {
        if(m_used_size != 0)
        {
            gl_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
        }
    }
}