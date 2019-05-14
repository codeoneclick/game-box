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
        
        mtl_texture_impl(ui32 width, ui32 height, void* pixels, ui32 format);
        mtl_texture_impl(ui32 size, std::array<ui8*, 6> pixels, ui32 format = gl::constant::rgba_t);
        mtl_texture_impl(void* texture_descriptor);
        ~mtl_texture_impl();
        
        void* get_mtl_raw_texture_ptr() const override;
    };
    
    mtl_texture_impl::mtl_texture_impl(ui32 width, ui32 height, void* pixels, ui32 format)
    {
        id<MTLDevice> mtl_raw_device = (__bridge id<MTLDevice>)gb::mtl_device::get_instance()->get_mtl_raw_device_ptr();
        
        MTLPixelFormat pixel_format = MTLPixelFormatRGBA8Unorm;
        if (format == gl::constant::rgba_t)
        {
            pixel_format = MTLPixelFormatRGBA8Unorm;
        }
        else if (format == gl::constant::red)
        {
            pixel_format = MTLPixelFormatR8Unorm;
        }
        
        m_texture_descriptor = [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:pixel_format
                                                                                  width:width
                                                                                 height:height
                                                                              mipmapped:NO];
        m_texture = [mtl_raw_device newTextureWithDescriptor:m_texture_descriptor];
        
        if (pixels)
        {
            ui32 bytes_per_row = width * (format == gl::constant::rgba_t ? 4 : 1);
            MTLRegion region = MTLRegionMake2D(0, 0, width, height);
            [m_texture replaceRegion:region mipmapLevel:0 withBytes:pixels bytesPerRow:bytes_per_row];
        }
    }
    
    mtl_texture_impl::mtl_texture_impl(ui32 size, std::array<ui8*, 6> pixels, ui32 format)
    {
        id<MTLDevice> mtl_raw_device = (__bridge id<MTLDevice>)gb::mtl_device::get_instance()->get_mtl_raw_device_ptr();
        
        MTLPixelFormat pixel_format = MTLPixelFormatRGBA8Unorm;
        if (format == gl::constant::rgba_t)
        {
            pixel_format = MTLPixelFormatRGBA8Unorm;
        }
        else if (format == gl::constant::red)
        {
            pixel_format = MTLPixelFormatR8Unorm;
        }
        
        m_texture_descriptor = [MTLTextureDescriptor textureCubeDescriptorWithPixelFormat:pixel_format
                                                                                     size:size mipmapped:NO];
        m_texture = [mtl_raw_device newTextureWithDescriptor:m_texture_descriptor];
        MTLRegion region = MTLRegionMake2D(0, 0, size, size);
        ui32 bytes_per_row = size * (format == gl::constant::rgba_t ? 4 : 1);
        ui32 bytes_per_image = bytes_per_row * size;
        
        for (ui32 slice = 0; slice < 6; ++slice)
        {
            [m_texture replaceRegion:region mipmapLevel:0 slice:slice withBytes:pixels.at(slice) bytesPerRow:bytes_per_row
                     bytesPerImage:bytes_per_image];
            
        }
    }
    
    mtl_texture_impl::mtl_texture_impl(void* texture_descriptor)
    {
        id<MTLDevice> mtl_raw_device = (__bridge id<MTLDevice>)gb::mtl_device::get_instance()->get_mtl_raw_device_ptr();
        MTLTextureDescriptor* mtl_texture_descriptor = (__bridge MTLTextureDescriptor*)texture_descriptor;
        m_texture_descriptor = mtl_texture_descriptor;
        m_texture = [mtl_raw_device newTextureWithDescriptor:m_texture_descriptor];
    }
    
    mtl_texture_impl::~mtl_texture_impl()
    {
        
    }
    
    void* mtl_texture_impl::get_mtl_raw_texture_ptr() const
    {
        return (__bridge void*)m_texture;
    }
    
    mtl_texture::mtl_texture(ui32 width, ui32 height, void* pixels, ui32 format)
    {
        m_texture_impl = std::make_shared<mtl_texture_impl>(width, height, pixels, format);
    }
    
    mtl_texture::mtl_texture(ui32 size, std::array<ui8*, 6> pixels, ui32 format)
    {
        m_texture_impl = std::make_shared<mtl_texture_impl>(size, pixels, format);
    }
    
    mtl_texture::mtl_texture(void* mtl_texture_descriptor)
    {
        m_texture_impl = std::make_shared<mtl_texture_impl>(mtl_texture_descriptor);
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
