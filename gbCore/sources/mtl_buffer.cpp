//
//  mtl_buffer.cpp
//  gbCore
//
//  Created by serhii.s on 3/11/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "mtl_buffer.h"
#include "mtl_device.h"

#if USED_GRAPHICS_API == METAL_API

#include <MetalKit/MetalKit.h>

namespace gb
{
    class mtl_buffer_impl : public i_mtl_buffer_impl
    {
    private:
        
        id<MTLBuffer> m_buffer = nil;
        void* m_data = nullptr;
        ui32 m_size = 0;
        
    protected:
        
    public:
        
        mtl_buffer_impl(ui32 size);
        mtl_buffer_impl(void* data, ui32 size);
        ~mtl_buffer_impl();
        
        void update(void* data, ui32 size);
        
        void* get_mtl_raw_buffer_ptr() const override;
    };
    
    mtl_buffer_impl::mtl_buffer_impl(ui32 size)
    {
        id<MTLDevice> mtl_raw_device = (__bridge id<MTLDevice>)gb::mtl_device::get_instance()->get_mtl_raw_device_ptr();
        m_buffer = [mtl_raw_device newBufferWithLength:size options:0];
        m_size = size;
    }
    
    mtl_buffer_impl::mtl_buffer_impl(void* data, ui32 size)
    {
        id<MTLDevice> mtl_raw_device = (__bridge id<MTLDevice>)gb::mtl_device::get_instance()->get_mtl_raw_device_ptr();
        m_buffer = [mtl_raw_device newBufferWithBytes:data length:size options:MTLResourceOptionCPUCacheModeDefault];
        m_size = size;
    }
    
    mtl_buffer_impl::~mtl_buffer_impl()
    {
        
    }
    
    void mtl_buffer_impl::update(void* data, ui32 size)
    {
        assert(size <= m_size);
        if (size <= m_size)
        {
            uint8_t* buffer_ptr = (uint8_t *)[m_buffer contents];
            memcpy(buffer_ptr, data, size);
        }
    }
    
    void* mtl_buffer_impl::get_mtl_raw_buffer_ptr() const
    {
        return (__bridge void*)m_buffer;
    }
    
    mtl_buffer::mtl_buffer(ui32 size)
    {
        m_buffer_impl = std::make_shared<mtl_buffer_impl>(size);
    }
    
    mtl_buffer::mtl_buffer(void* data, ui32 size)
    {
        m_buffer_impl = std::make_shared<mtl_buffer_impl>(data, size);
    }
    
    mtl_buffer::~mtl_buffer()
    {
        
    }
    
    void mtl_buffer::update(void* data, ui32 size)
    {
        impl_as<mtl_buffer_impl>()->update(data, size);
    }
    
    void* mtl_buffer::get_mtl_raw_buffer_ptr() const
    {
        return impl_as<mtl_buffer_impl>()->get_mtl_raw_buffer_ptr();
    }
}

#endif
