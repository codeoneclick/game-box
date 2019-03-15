//
//  mtl_depth_stencil_state.cpp
//  gbCore
//
//  Created by serhii.s on 3/5/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "mtl_depth_stencil_state.h"
#include "mtl_device.h"

#if USED_GRAPHICS_API == METAL_API

#include <MetalKit/MetalKit.h>

namespace gb
{
    class mtl_depth_stencil_state_impl : public i_mtl_depth_stencil_state_impl
    {
    private:
        
        id<MTLDepthStencilState> m_depth_stencil_state = nil;
        MTLDepthStencilDescriptor* m_depth_stencil_state_descriptor = nil;
        
    protected:
        
    public:
        
        mtl_depth_stencil_state_impl();
        ~mtl_depth_stencil_state_impl();
        
        void* get_mtl_raw_depth_stencil_state_ptr() const override;
    };
    
    mtl_depth_stencil_state_impl::mtl_depth_stencil_state_impl()
    {
        id<MTLDevice> mtl_device = (__bridge id<MTLDevice>)gb::mtl_device::get_instance()->get_mtl_raw_device_ptr();
        
        m_depth_stencil_state_descriptor = [[MTLDepthStencilDescriptor alloc] init];
        m_depth_stencil_state_descriptor.depthCompareFunction = MTLCompareFunctionLessEqual;
        m_depth_stencil_state_descriptor.depthWriteEnabled = YES;
        m_depth_stencil_state = [mtl_device newDepthStencilStateWithDescriptor:m_depth_stencil_state_descriptor];
    }
    
    mtl_depth_stencil_state_impl::~mtl_depth_stencil_state_impl()
    {
        
    }
    
    void* mtl_depth_stencil_state_impl::get_mtl_raw_depth_stencil_state_ptr() const
    {
        return (__bridge void*)m_depth_stencil_state;
    }
    
    mtl_depth_stencil_state::mtl_depth_stencil_state()
    {
        m_mtl_depth_stencil_state_impl = std::make_shared<mtl_depth_stencil_state_impl>();
    }
    
    mtl_depth_stencil_state::~mtl_depth_stencil_state()
    {
        
    }
    
    void* mtl_depth_stencil_state::get_mtl_raw_depth_stencil_state_ptr() const
    {
        return impl_as<mtl_depth_stencil_state_impl>()->get_mtl_raw_depth_stencil_state_ptr();
    }
}

#endif
