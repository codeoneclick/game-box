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

@interface mtl_device_wrapper : NSObject

@property(nonatomic, readonly, nonnull) id<MTLDevice> m_device;
@property(nonatomic, readonly, nonnull) id<MTLCommandQueue> m_command_queue;
@property(nonatomic, readonly, nonnull) id<MTLLibrary> m_library;
@property(nonatomic, readonly, nonnull) id<MTLCommandBuffer> m_command_buffer;
@property(nonatomic, readonly, nonnull) id<MTLRenderCommandEncoder> m_render_command_encoder;
@property(nonatomic, readonly, nonnull) dispatch_semaphore_t m_render_commands_semaphore;
@property(nonatomic, readonly) BOOL m_should_reconstruct_render_encoder;

+ (mtl_device_wrapper* )shared_instance;

- (void)bind;
- (void)unbind:(id<CAMetalDrawable>) drawable;
- (void)reconstruct_render_encoder:(MTLRenderPassDescriptor*) render_descriptor;

@end

@implementation mtl_device_wrapper

+ (mtl_device_wrapper* )shared_instance
{
    static mtl_device_wrapper *instance = nil;
    static dispatch_once_t once;
    dispatch_once(&once, ^{
        instance = [[self alloc] init];
    });
    return instance;
}

- (instancetype)init
{
    self = [super init];
    if(self)
    {
        _m_device = MTLCreateSystemDefaultDevice();
        _m_command_queue = [_m_device newCommandQueue];
        _m_library = [_m_device newDefaultLibrary];
        _m_render_commands_semaphore = dispatch_semaphore_create(2);
        _m_should_reconstruct_render_encoder = YES;
        
    }
    return self;
}

- (void)bind
{
    dispatch_semaphore_wait(_m_render_commands_semaphore, DISPATCH_TIME_FOREVER);
    _m_command_buffer = [_m_command_queue commandBuffer];
    _m_command_buffer.label = @"command buffer";
    
    dispatch_semaphore_t block_semaphore = _m_render_commands_semaphore;
    [_m_command_buffer addCompletedHandler:^(id<MTLCommandBuffer> command_buffer) {
        NSError* error = command_buffer.error;
        if (error)
        {
            NSLog(@"%@", [error localizedDescription]);
            std::cout<<[error code]<<std::endl;
            assert(false);
        }
        dispatch_semaphore_signal(block_semaphore);
    }];
}

- (void)unbind:(id<CAMetalDrawable>) drawable
{
    _m_should_reconstruct_render_encoder = YES;
    [_m_command_buffer enqueue];
    
    if (drawable.texture == nil)
    {
        [_m_command_buffer commit];
        return;
    }
    
    if (drawable)
    {
        [_m_command_buffer presentDrawable:drawable];
    }
    [_m_command_buffer commit];
}

- (void)reconstruct_render_encoder:(MTLRenderPassDescriptor*) render_descriptor
{
    if (_m_should_reconstruct_render_encoder)
    {
        _m_render_command_encoder = [_m_command_buffer renderCommandEncoderWithDescriptor:render_descriptor];
        _m_should_reconstruct_render_encoder = NO;
    }
}

@end

namespace gb
{
    class mtl_device_impl : public i_mtl_device_impl
    {
    private:
        
        MTKView* m_hwnd = nil;
        
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
        while (dispatch_semaphore_signal([mtl_device_wrapper shared_instance].m_render_commands_semaphore) != 0) {};
    }
    
    std::shared_ptr<mtl_device_impl> mtl_device_impl::construct()
    {
        const auto device_impl = std::make_shared<mtl_device_impl>();
        [mtl_device_wrapper shared_instance];
        return device_impl;
    }

    void mtl_device_impl::init(MTKView* hwnd)
    {
        hwnd.device = [mtl_device_wrapper shared_instance].m_device;
        m_hwnd = hwnd;
    }
    
    void* mtl_device_impl::get_mtl_raw_device_ptr() const
    {
        return (__bridge void*)[mtl_device_wrapper shared_instance].m_device;
    }
    
    void* mtl_device_impl::get_mtl_raw_library_ptr() const
    {
        return (__bridge void*)[mtl_device_wrapper shared_instance].m_library;
    }
    
    void* mtl_device_impl::get_mtl_raw_command_queue_ptr() const
    {
        return (__bridge void*)[mtl_device_wrapper shared_instance].m_command_queue;
    }
    
    void* mtl_device_impl::get_mtl_raw_command_buffer_ptr() const
    {
        return (__bridge void*)[mtl_device_wrapper shared_instance].m_command_buffer;
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
        if (m_hwnd.currentRenderPassDescriptor)
        {
            [[mtl_device_wrapper shared_instance] reconstruct_render_encoder:m_hwnd.currentRenderPassDescriptor];
        }
        return (__bridge void*)[mtl_device_wrapper shared_instance].m_render_command_encoder;
    }
    
    void* mtl_device_impl::get_mtl_raw_color_attachment_ptr() const
    {
        if(m_hwnd.currentDrawable == nil)
        {
            assert(false);
        }
        
        id<MTLTexture> color_attachment = m_hwnd.currentDrawable.texture;
        return (__bridge void*)color_attachment;
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
        [[mtl_device_wrapper shared_instance] bind];
    }
    
    void mtl_device_impl::unbind()
    {
        // id<CAMetalDrawable> drawable = [((CAMetalLayer *)[m_hwnd layer]) nextDrawable];
        id<CAMetalDrawable> drawable = [m_hwnd currentDrawable];
        [[mtl_device_wrapper shared_instance] unbind:drawable];
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
