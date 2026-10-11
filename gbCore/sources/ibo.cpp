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

#if USED_GRAPHICS_API == METAL_API

#include "mtl_buffer.h"

#endif

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

#if USED_GRAPHICS_API == VULKAN_API

        if (m_mode == gl::constant::dynamic_draw)
        {
            m_dynamic_frames.resize(vk_device::get_instance()->get_draw_cmd_buffers().size());
        }
        else
        {
		m_staging_buffer = std::make_shared<vk_buffer>();
		VkResult result = vk_utils::create_buffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, m_staging_buffer, sizeof(ui16) * m_allocated_size);
		assert(result == VK_SUCCESS);

		m_main_buffer = std::make_shared<vk_buffer>();
		result = vk_utils::create_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_main_buffer, sizeof(ui16) * m_allocated_size);
		assert(result == VK_SUCCESS);

        }

#endif
        
#if USED_GRAPHICS_API == METAL_API
        
        m_mtl_buffer_id = std::make_shared<mtl_buffer>(sizeof(ui16) * m_allocated_size);
        
#endif

        if(!m_is_using_batch)
        {
            gl::command::create_buffers(1, &m_handle);
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

        if(!m_is_using_batch && submit_to_vram)
        {
            gl::command::bind_buffer(gl::constant::element_array_buffer, m_handle);
            gl::command::push_buffer_data(gl::constant::element_array_buffer, sizeof(ui16) * m_used_size, m_data, m_mode);
        }

#if USED_GRAPHICS_API == VULKAN_API

        if (!m_is_using_batch && submit_to_vram)
        {
            const VkDeviceSize bytes = sizeof(ui16) * m_used_size;
            if (m_mode == gl::constant::dynamic_draw)
            {
                // Snapshot submitted data; unlock(..., false) must not change GPU contents.
                m_dynamic_data.resize(static_cast<std::size_t>(bytes));
                memcpy(m_dynamic_data.data(), m_data, static_cast<std::size_t>(bytes));
                ++m_dynamic_version;
            }
            else
            {
                if (!m_staging_buffer)
                {
                    m_staging_buffer = std::make_shared<vk_buffer>();
                    VK_CHECK(vk_utils::create_buffer(VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                        VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                        m_staging_buffer, sizeof(ui16) * m_allocated_size));
                }
                VK_CHECK(m_staging_buffer->map(bytes, 0));
                m_staging_buffer->copy_to(m_data, bytes);
                m_staging_buffer->unmap();
                vk_utils::copy_buffers(m_staging_buffer, m_main_buffer, bytes);
                m_staging_buffer.reset();
            }
        }

#endif
        
#if USED_GRAPHICS_API == METAL_API
        
        m_mtl_buffer_id->update(m_data, sizeof(ui16) * m_used_size);
        
#endif

        m_version++;
    }
    
    void ibo::bind() const
    {
        if(m_used_size != 0 && !m_is_using_batch)
        {

#if USED_GRAPHICS_API == VULKAN_API

			ui32 current_image_index = vk_device::get_instance()->get_current_image_index();
			VkCommandBuffer draw_cmd_buffer = vk_device::get_instance()->get_draw_cmd_buffer(current_image_index);
            VkBuffer buffer = VK_NULL_HANDLE;
            if (m_mode == gl::constant::dynamic_draw)
            {
                // The image fence was waited by make_current() before any draw.
                // Use another slot if this mesh changes between draws of the same frame.
                auto& frame = m_dynamic_frames[current_image_index];
                const ui64 frame_number = vk_device::get_instance()->get_frame_number(current_image_index);
                if (frame.frame_number != frame_number)
                {
                    frame.frame_number = frame_number;
                    frame.used_buffers = 0;
                }
                ui32 slot = frame.used_buffers == 0 ? 0 : frame.used_buffers - 1;
                if (frame.used_buffers == 0 || frame.versions[slot] != m_dynamic_version)
                {
                    slot = frame.used_buffers++;
                    if (slot == frame.buffers.size())
                    {
                        auto storage = std::make_shared<vk_buffer>();
                        VK_CHECK(vk_utils::create_buffer(VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
                            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                            storage, sizeof(ui16) * m_allocated_size));
                        VK_CHECK(storage->map());
                        frame.buffers.push_back(storage);
                        frame.versions.push_back(static_cast<ui32>(-1));
                    }
                    if (frame.versions[slot] != m_dynamic_version)
                    {
                        if (!m_dynamic_data.empty())
                        {
                            memcpy(frame.buffers[slot]->get_mapped_data(), m_dynamic_data.data(), m_dynamic_data.size());
                        }
                        frame.versions[slot] = m_dynamic_version;
                    }
                }
                buffer = frame.buffers[slot]->get_handler();
            }
            else
            {
                buffer = m_main_buffer->get_handler();
            }
			vkCmdBindIndexBuffer(draw_cmd_buffer, buffer, 0, VK_INDEX_TYPE_UINT16);

#endif

            gl::command::bind_buffer(gl::constant::element_array_buffer, m_handle);
        }
    }
    
    void ibo::unbind() const
    {
        if(m_used_size != 0 && !m_is_using_batch)
        {
            gl::command::bind_buffer(gl::constant::element_array_buffer, NULL);
        }
    }
    
#if USED_GRAPHICS_API == METAL_API
    
    mtl_buffer_shared_ptr ibo::get_mtl_buffer_id() const
    {
        return m_mtl_buffer_id;
    }
    
#endif
}
