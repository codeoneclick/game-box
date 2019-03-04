//
//  mtl_device.cpp
//  gbCore
//
//  Created by serhii.s on 3/1/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "mtl_device.h"

#if USED_GRAPHICS_API == METAL_API

#include <Cocoa/Cocoa.h>
#include <MetalKit/MetalKit.h>

namespace gb
{
    class mtl_device_impl : public i_mtl_device_impl
    {
    private:
        
        id<MTLDevice> m_device = nil;
        id<MTLCommandQueue> m_command_queue = nil;
        id<MTLLibrary> m_library = nil;
        
    protected:
        
    public:
        
        mtl_device_impl();
        ~mtl_device_impl();
        static std::shared_ptr<mtl_device_impl> construct();

        void setup(MTKView* hwnd);
    };
    
    mtl_device_impl::mtl_device_impl()
    {
        
    }
    
    mtl_device_impl::~mtl_device_impl()
    {
        
    }
    
    std::shared_ptr<mtl_device_impl> mtl_device_impl::construct()
    {
        const auto device_impl = std::make_shared<mtl_device_impl>();
        device_impl->m_device = MTLCreateSystemDefaultDevice();
        
        device_impl->m_command_queue = [device_impl->m_device newCommandQueue];
        device_impl->m_library = [device_impl->m_device newDefaultLibrary];
        
        return device_impl;
    }

    void mtl_device_impl::setup(MTKView* hwnd)
    {
        hwnd.device = m_device;
    }


/*id<MTLLibrary> mtl_library = [mtl_device newDefaultLibrary];
id<MTLFunction> vertex_function = [mtl_library newFunctionWithName:@"vertexShader"];
id<MTLFunction> fragment_function = [mtl_library newFunctionWithName:@"fragmentShader"];

MTLRenderPipelineDescriptor *pipeline_state_descriptor = [[MTLRenderPipelineDescriptor alloc] init];
pipeline_state_descriptor.label = @"mtl pipeline";
pipeline_state_descriptor.vertexFunction = vertex_function;
pipeline_state_descriptor.fragmentFunction = fragment_function;
pipeline_state_descriptor.colorAttachments[0].pixelFormat = mtl_view.colorPixelFormat;

NSError *error = NULL;
id<MTLRenderPipelineState> pipeline_state = [mtl_device newRenderPipelineStateWithDescriptor:pipeline_state_descriptor
                                                                                       error:&error];
if (!pipeline_state)
{
    NSLog(@"failed to created pipeline state, error %@", error);
    return;
}

id<MTLCommandQueue> command_Queue = [mtl_device newCommandQueue];*/

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
    
    void mtl_device::setup(const void* hwnd)
    {
        MTKView *mtl_hwnd = (__bridge MTKView *)hwnd;
        impl_as<mtl_device_impl>()->setup(mtl_hwnd);
    }
}

#endif
