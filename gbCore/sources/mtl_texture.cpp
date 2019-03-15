//
//  mtl_texture.cpp
//  gbCore
//
//  Created by serhii.s on 3/6/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "mtl_texture.h"
#include "mtl_device.h"

#if USED_GRAPHICS_API == METAL_API

#include <MetalKit/MetalKit.h>

namespace gb
{
    class mtl_texture_impl : public i_mtl_texture_impl
    {
    private:
        
        id<MTLTexture> m_texture = nil;
        MTLTextureDescriptor* m_texture_descriptor = nil;
        
    protected:
        
    public:
        
        mtl_texture_impl(ui32 width, ui32 height, void* pixels);
        ~mtl_texture_impl();
        
        void* get_mtl_raw_texture_ptr() const override;
    };
    
    mtl_texture_impl::mtl_texture_impl(ui32 width, ui32 height, void* pixels)
    {
        id<MTLDevice> mtl_raw_device = (__bridge id<MTLDevice>)gb::mtl_device::get_instance()->get_mtl_raw_device_ptr();
        
        m_texture_descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm
                                                                                  width:width
                                                                                 height:height
                                                                              mipmapped:NO];
        m_texture = [mtl_raw_device newTextureWithDescriptor:m_texture_descriptor];
        MTLRegion region = MTLRegionMake2D(0, 0, width, height);
        [m_texture replaceRegion:region mipmapLevel:0 withBytes:pixels bytesPerRow:width * 4];
    }
    
    mtl_texture_impl::~mtl_texture_impl()
    {
        
    }
    
    void* mtl_texture_impl::get_mtl_raw_texture_ptr() const
    {
        return (__bridge void*)m_texture;
    }
    
    mtl_texture::mtl_texture(ui32 width, ui32 height, void* pixels)
    {
        m_texture_impl = std::make_shared<mtl_texture_impl>(width, height, pixels);
    }
    
    mtl_texture::~mtl_texture()
    {
        
    }
    
    void* mtl_texture::get_mtl_raw_texture_ptr() const
    {
        return impl_as<mtl_texture_impl>()->get_mtl_raw_texture_ptr();
    }
}

#endif
