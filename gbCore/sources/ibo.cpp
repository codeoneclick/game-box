//
//  ibo.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ibo.h"
#include "vk_device.h"
#include "vk_utils.h"

namespace gb
{
    std::queue<ui32> ibo::m_handlers_graveyard;
    std::mutex ibo::m_graveyard_mutex;
    
    ibo::ibo(ui32 size, ui32 mode, bool is_using_batch, ui16* external_data) :
    m_handle(0),
    m_allocated_size(size),
    m_used_size(0),
    m_mode(mode),
    m_is_using_batch(is_using_batch),
    m_version(0)
    {
        assert(m_allocated_size != 0);

		m_type = e_resource_transfering_data_type_ibo;

		m_staging_buffer = std::make_shared<vk_buffer>();
		VkResult result = vk_utils::create_buffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_staging_buffer, sizeof(ui16) * m_allocated_size);
		assert(result == VK_SUCCESS);

		m_main_buffer = std::make_shared<vk_buffer>();
		result = vk_utils::create_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_main_buffer, sizeof(ui16) * m_allocated_size);
		assert(result == VK_SUCCESS);

        if(!m_is_using_batch)
        {
            gl_create_buffers(1, &m_handle);
        }
        
        if(!external_data)
        {
            m_data = new ui16[m_allocated_size];
            memset(m_data, 0x0, sizeof(ui16) * m_allocated_size);
        }
        else
        {
            m_data = external_data;
        }
        m_is_external_data = external_data != nullptr;
    }
    
    ibo::~ibo()
    {
        if(!m_is_using_batch)
        {
            ibo::add_to_graveyard(m_handle);
        }
        if(!m_is_external_data)
        {
            delete[] m_data;
        }
    }
    
    void ibo::add_to_graveyard(ui32 handler)
    {
        std::lock_guard<std::mutex> guard(m_graveyard_mutex);
        m_handlers_graveyard.push(handler);
    }
    
    ui32 ibo::get_allocated_size() const
    {
        return m_allocated_size;
    }
    
    ui32 ibo::get_used_size() const
    {
        return m_used_size;
    }
    
    ui16* ibo::lock() const
    {
        return m_data;
    }
    
    ui32 ibo::get_id() const
    {
        return m_handle;
    }
    
    ui32 ibo::get_version() const
    {
        return m_version;
    }
    
    bool ibo::is_using_batch() const
    {
        return m_is_using_batch;
    }
    
    void ibo::unlock(ui32 size, bool submit_to_vram)
    {
        assert(m_data != nullptr);
        assert(m_allocated_size != 0);
        m_used_size = size > 0 && size < m_allocated_size ? size : m_allocated_size;

#if !defined(__NO_RENDER__)
        
        if(!m_is_using_batch && submit_to_vram)
        {
            gl_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
            gl_push_buffer_data(GL_ELEMENT_ARRAY_BUFFER, sizeof(ui16) * m_used_size, m_data, m_mode);
        }

#endif

		m_staging_buffer->map(sizeof(ui16) * m_used_size, 0);
		m_staging_buffer->copy_to(m_data, sizeof(ui16) * m_used_size);
		m_staging_buffer->unmap();

		vk_utils::copy_buffers(m_staging_buffer, m_main_buffer);

        m_version++;
    }
    
    void ibo::bind() const
    {
#if !defined(__NO_RENDER__)

        if(m_used_size != 0 && !m_is_using_batch)
        {
			ui32 current_image_index = vk_device::get_instance()->get_current_image_index();
			VkCommandBuffer draw_cmd_buffer = vk_device::get_instance()->get_draw_cmd_buffer(current_image_index);
			VkBuffer buffer = m_main_buffer->get_handler();
			vkCmdBindIndexBuffer(draw_cmd_buffer, buffer, 0, VK_INDEX_TYPE_UINT16);

            gl_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, m_handle);
        }

#endif
    }
    
    void ibo::unbind() const
    {
#if !defined(__NO_RENDER__)

        if(m_used_size != 0 && !m_is_using_batch)
        {
            gl_bind_buffer(GL_ELEMENT_ARRAY_BUFFER, NULL);
        }

#endif
    }
}
