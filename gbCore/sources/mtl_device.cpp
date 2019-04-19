//
//  mtl_device.cpp
//  gbCore
//
//  Created by serhii.s on 3/1/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "mtl_device.h"
#include "mtl_render_pass_descriptor.h"

#if USED_GRAPHICS_API == METAL_API

#include <MetalKit/MetalKit.h>

namespace gb
{
    class mtl_device_impl : public i_mtl_device_impl
    {
    private:
        
        MTKView* m_hwnd = nil;
        id<MTLDevice> m_device = nil;
        id<MTLCommandQueue> m_command_queue = nil;
        id<MTLLibrary> m_library = nil;
        id<MTLCommandBuffer> m_command_buffer = nil;
        id<MTLRenderCommandEncoder> m_render_command_encoder = nil;
        dispatch_semaphore_t m_render_commands_semaphore;
        
    protected:
        
    public:
        
        mtl_device_impl();
        ~mtl_device_impl();
        
        static std::shared_ptr<mtl_device_impl> construct();

        void init(MTKView* hwnd);
        
        void* get_mtl_raw_device_ptr() const override;
        void* get_mtl_raw_library_ptr() const override;
        void* get_mtl_raw_command_queue_ptr() const override;
        void* get_mtl_raw_command_buffer_ptr() const override;
        void* get_mtl_raw_main_render_encoder() override;
        void* get_mtl_raw_color_attachment_ptr() const override;
        void* get_mtl_raw_depth_stencil_attachment_ptr() const override;
        
        ui64 get_samples_count() const override;
        
        ui64 get_color_pixel_format() const override;
        ui64 get_depth_stencil_pixel_format() const override;
        
        void bind() override;
        void unbind() override;
    };
    
    mtl_device_impl::mtl_device_impl()
    {
        
    }
    
    mtl_device_impl::~mtl_device_impl()
    {
        while (dispatch_semaphore_signal(m_render_commands_semaphore) != 0) {};
    }
    
    std::shared_ptr<mtl_device_impl> mtl_device_impl::construct()
    {
        const auto device_impl = std::make_shared<mtl_device_impl>();
        device_impl->m_device = MTLCreateSystemDefaultDevice();
        
        device_impl->m_command_queue = [device_impl->m_device newCommandQueue];
        device_impl->m_library = [device_impl->m_device newDefaultLibrary];
        device_impl->m_render_commands_semaphore = dispatch_semaphore_create(3);
        
        return device_impl;
    }

    void mtl_device_impl::init(MTKView* hwnd)
    {
        hwnd.device = m_device;
        m_hwnd = hwnd;
    }
    
    void* mtl_device_impl::get_mtl_raw_device_ptr() const
    {
        return (__bridge void*)m_device;
    }
    
    void* mtl_device_impl::get_mtl_raw_library_ptr() const
    {
        return (__bridge void*)m_library;
    }
    
    void* mtl_device_impl::get_mtl_raw_command_queue_ptr() const
    {
        return (__bridge void*)m_command_queue;
    }
    
    void* mtl_device_impl::get_mtl_raw_command_buffer_ptr() const
    {
        return (__bridge void*)m_command_buffer;
    }
    
    ui64 mtl_device_impl::get_samples_count() const
    {
        return m_hwnd.sampleCount;
    }
    
    ui64 mtl_device_impl::get_color_pixel_format() const
    {
        return m_hwnd.colorPixelFormat;
    }
    
    ui64 mtl_device_impl::get_depth_stencil_pixel_format() const
    {
        return m_hwnd.depthStencilPixelFormat;
    }
    
    void* mtl_device_impl::get_mtl_raw_main_render_encoder()
    {
        MTLRenderPassDescriptor *render_pass_descriptor = m_hwnd.currentRenderPassDescriptor;
        if (render_pass_descriptor)
        {
            if (m_render_command_encoder == nil)
            {
                m_render_command_encoder = [m_command_buffer renderCommandEncoderWithDescriptor:render_pass_descriptor];
            }
            return (__bridge void*)m_render_command_encoder;
        }
        return nullptr;
    }
    
    void* mtl_device_impl::get_mtl_raw_color_attachment_ptr() const
    {
        if(m_hwnd.currentDrawable == nil)
        {
            assert(false);
        }
        
#if defined(__TVOS__)

        return nullptr;
        
#else
        
        id<MTLTexture> color_attachment = m_hwnd.currentDrawable.texture;
        return (__bridge void*)color_attachment;
        
#endif
        
    }
    
    void* mtl_device_impl::get_mtl_raw_depth_stencil_attachment_ptr() const
    {
        if(m_hwnd.depthStencilTexture == nil)
        {
            assert(false);
        }
        
        id<MTLTexture> depth_stencil_attachment = m_hwnd.depthStencilTexture;
        return (__bridge void*)depth_stencil_attachment;
    }
    
    void mtl_device_impl::bind()
    {
        dispatch_semaphore_wait(m_render_commands_semaphore, DISPATCH_TIME_FOREVER);
        
        m_command_buffer = [m_command_queue commandBuffer];
        m_command_buffer.label = @"command buffer";
    }
    
    void mtl_device_impl::unbind()
    {
        m_render_command_encoder = nil;
        dispatch_semaphore_t block_semaphore = m_render_commands_semaphore;
        [m_command_buffer addCompletedHandler:^(id<MTLCommandBuffer> command_buffer) {
            NSError* error = command_buffer.error;
            if (error)
            {
                NSLog(@"%@", [error localizedDescription]);
                std::cout<<[error code]<<std::endl;
            }
            dispatch_semaphore_signal(block_semaphore);
        }];
        
        if(m_hwnd.currentDrawable)
        {
            [m_command_buffer presentDrawable:m_hwnd.currentDrawable];
        }
        [m_command_buffer commit];
    }

    std::shared_ptr<mtl_device> mtl_device::m_instance = nullptr;
    
    std::shared_ptr<mtl_device> mtl_device::get_instance()
    {
        if (!m_instance)
        {
            m_instance = mtl_device::construct();
        }
        return m_instance;
    }
    
    mtl_device::~mtl_device()
    {

    }
    
    std::shared_ptr<mtl_device> mtl_device::construct()
    {
        const auto device = std::make_shared<mtl_device>();
        device->m_device_impl = mtl_device_impl::construct();
        return device;
    }
    
    void mtl_device::init(const void* hwnd)
    {
        MTKView *mtl_hwnd = (__bridge MTKView *)hwnd;
        impl_as<mtl_device_impl>()->init(mtl_hwnd);
    }
    
    void* mtl_device::get_mtl_raw_device_ptr() const
    {
        return impl_as<mtl_device_impl>()->get_mtl_raw_device_ptr();
    }
    
    void* mtl_device::get_mtl_raw_library_ptr() const
    {
        return impl_as<mtl_device_impl>()->get_mtl_raw_library_ptr();
    }
    
    void* mtl_device::get_mtl_raw_command_queue_ptr() const
    {
        return impl_as<mtl_device_impl>()->get_mtl_raw_command_queue_ptr();
    }
    
    void* mtl_device::get_mtl_raw_command_buffer_ptr() const
    {
        return impl_as<mtl_device_impl>()->get_mtl_raw_command_buffer_ptr();
    }
    
    ui64 mtl_device::get_samples_count() const
    {
        return impl_as<mtl_device_impl>()->get_samples_count();
    }
    
    ui64 mtl_device::get_color_pixel_format() const
    {
        return impl_as<mtl_device_impl>()->get_color_pixel_format();
    }
    
    ui64 mtl_device::get_depth_stencil_pixel_format() const
    {
        return impl_as<mtl_device_impl>()->get_depth_stencil_pixel_format();
    }
    
    void* mtl_device::get_mtl_render_encoder(const std::string& guid) const
    {
        assert(m_current_render_pass_descriptor);
        return m_current_render_pass_descriptor->get_mtl_render_encoder(guid);
    }
    
    void* mtl_device::get_mtl_raw_main_render_encoder()
    {
        return impl_as<mtl_device_impl>()->get_mtl_raw_main_render_encoder();
    }
    
    void* mtl_device::get_mtl_raw_color_attachment_ptr() const
    {
        return impl_as<mtl_device_impl>()->get_mtl_raw_color_attachment_ptr();
    }
    
    void* mtl_device::get_mtl_raw_depth_stencil_attachment_ptr() const
    {
        return impl_as<mtl_device_impl>()->get_mtl_raw_depth_stencil_attachment_ptr();
    }
    
    void mtl_device::bind()
    {
        impl_as<mtl_device_impl>()->bind();
    }
    
    void mtl_device::unbind()
    {
        impl_as<mtl_device_impl>()->unbind();
    }
    
    void mtl_device::set_current_render_pass_descriptor(const mtl_render_pass_descriptor_shared_ptr& render_pass_descriptor)
    {
        m_current_render_pass_descriptor = render_pass_descriptor;
    }
    
    mtl_render_pass_descriptor_shared_ptr mtl_device::get_current_render_pass_descriptor() const
    {
        return m_current_render_pass_descriptor;
    }
}

#endif
