//
//  ibo.h
//  gbCore
//
//  Created by sergey.sergeev on 8/13/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "resource.h"
#include "vk_buffer.h"

namespace gb
{
    class ibo final : public resource_transfering_data
    {
    private:
        
        friend class ogl_graveyard_controller;

		VkDevice m_vk_device = VK_NULL_HANDLE;
        
    protected:
        
        static std::mutex m_graveyard_mutex;
        static std::queue<ui32> m_handlers_graveyard;
        void add_to_graveyard(ui32 handler);

		std::shared_ptr<vk_buffer> m_vk_handle;
        
        ui32 m_handle;
        ui32 m_version;
        
        ui16* m_data;
        ui32 m_allocated_size;
        ui32 m_used_size;
        
        ui32 m_mode;
        bool m_is_using_batch;
        bool m_is_external_data;
        
    public:
        
        ibo(ui32 size, ui32 mode, bool is_using_batch = false, ui16* external_data = nullptr);
        ~ibo();
        
        ui32 get_id() const;
        ui32 get_version() const;
        
        bool is_using_batch() const;
        
        ui32 get_allocated_size() const;
        ui32 get_used_size() const;
        
        ui16* lock() const;
        void unlock(ui32 size = 0, bool submit_to_vram = true);
        
        void bind() const;
        void unbind() const;
    };
};
