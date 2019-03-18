//
//  mtl_render_pass_descriptor.cpp
//  gbCore
//
//  Created by serhii.s on 3/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "mtl_render_pass_descriptor.h"
#include "mtl_device.h"
#include "mtl_texture.h"
#include "texture.h"

#if USED_GRAPHICS_API == METAL_API

#include <MetalKit/MetalKit.h>

namespace gb
{
    class mtl_render_pass_descriptor_impl : public i_mtl_render_pass_descriptor_impl
    {
    private:
        
        NSString* m_name;
        MTLRenderPassDescriptor* m_render_pass_descriptor = nil;
        id<MTLRenderCommandEncoder> m_render_command_encoder = nil;

        std::vector<ui64> m_color_attachments_pixel_format;
        std::vector<texture_shared_ptr> m_color_attachments_texture;
        bool m_is_main_render_pass_descriptor = false;
        
    protected:
        
    public:
        
        mtl_render_pass_descriptor_impl(const std::string& name, ui32 width, ui32 height);
        mtl_render_pass_descriptor_impl(const std::string& name, void* mtl_raw_color_attachment_ptr, void* mtl_raw_depth_stencil_attachment_ptr);
        ~mtl_render_pass_descriptor_impl();
        
        void* get_mtl_render_pass_descriptor_ptr() const override;
        void* get_mtl_render_commnad_encoder() const override;
        void* get_mtl_render_encoder(const std::string& guid) const override;
        
        bool is_color_attachment_exist(i32 index) const override;
        ui64 get_color_attachment_pixel_format(i32 index) const override;
        
        std::vector<texture_shared_ptr> get_color_attachments_texture() override;
        
        void bind() override;
        void unbind() override;
    };
    
    mtl_render_pass_descriptor_impl::mtl_render_pass_descriptor_impl(const std::string& name, ui32 width, ui32 height)
    {
        m_name = [NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding];
        m_render_pass_descriptor = [MTLRenderPassDescriptor new];
        
        id<MTLTexture> mtl_depth_stencil_attachment = (__bridge id<MTLTexture>)gb::mtl_device::get_instance()->get_mtl_raw_depth_stencil_attachment_ptr();
        
        MTLTextureDescriptor *attachment_texture_descriptor =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm_sRGB
                                                           width:width
                                                          height:height
                                                       mipmapped:NO];
        
        attachment_texture_descriptor.textureType = MTLTextureType2D;
        attachment_texture_descriptor.usage |= MTLTextureUsageRenderTarget;
        attachment_texture_descriptor.storageMode = MTLStorageModePrivate;
        
        attachment_texture_descriptor.pixelFormat = MTLPixelFormatRGBA8Unorm_sRGB;
        auto mtl_texture_wrapper = std::make_shared<mtl_texture>((__bridge void*)attachment_texture_descriptor);
        id<MTLTexture> mtl_raw_texture = (__bridge id<MTLTexture>)mtl_texture_wrapper->get_mtl_raw_texture_ptr();
        std::string mtl_texture_guid = name;
        mtl_texture_guid.append(".attachment_01");
        mtl_raw_texture.label = [NSString stringWithCString:mtl_texture_guid.c_str() encoding:NSUTF8StringEncoding];
        auto texture = gb::texture::construct(mtl_texture_guid, mtl_texture_wrapper, width, height);
        m_color_attachments_texture.push_back(texture);
        m_color_attachments_pixel_format.push_back(MTLPixelFormatRGBA8Unorm_sRGB);
        m_render_pass_descriptor.colorAttachments[0].texture = mtl_raw_texture;
        
        attachment_texture_descriptor.pixelFormat = MTLPixelFormatRGBA8Snorm;
        mtl_texture_wrapper = std::make_shared<mtl_texture>((__bridge void*)attachment_texture_descriptor);
        mtl_raw_texture = (__bridge id<MTLTexture>)mtl_texture_wrapper->get_mtl_raw_texture_ptr();
        mtl_texture_guid = name;
        mtl_texture_guid.append(".attachment_02");
        mtl_raw_texture.label = [NSString stringWithCString:mtl_texture_guid.c_str() encoding:NSUTF8StringEncoding];
        texture = gb::texture::construct(mtl_texture_guid, mtl_texture_wrapper, width, height);
        m_color_attachments_texture.push_back(texture);
        m_color_attachments_pixel_format.push_back(MTLPixelFormatRGBA8Snorm);
        m_render_pass_descriptor.colorAttachments[1].texture = mtl_raw_texture;
        
        attachment_texture_descriptor.pixelFormat = MTLPixelFormatR32Float;
        mtl_texture_wrapper = std::make_shared<mtl_texture>((__bridge void*)attachment_texture_descriptor);
        mtl_raw_texture = (__bridge id<MTLTexture>)mtl_texture_wrapper->get_mtl_raw_texture_ptr();
        mtl_texture_guid = name;
        mtl_texture_guid.append(".attachment_03");
        mtl_raw_texture.label = [NSString stringWithCString:mtl_texture_guid.c_str() encoding:NSUTF8StringEncoding];
        texture = gb::texture::construct(mtl_texture_guid, mtl_texture_wrapper, width, height);
        m_color_attachments_texture.push_back(texture);
        m_color_attachments_pixel_format.push_back(MTLPixelFormatR32Float);
        m_render_pass_descriptor.colorAttachments[2].texture = mtl_raw_texture;
        
        m_render_pass_descriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);
        m_render_pass_descriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        m_render_pass_descriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        m_render_pass_descriptor.colorAttachments[1].clearColor = MTLClearColorMake(0, 0, 0, 1);
        m_render_pass_descriptor.colorAttachments[1].loadAction = MTLLoadActionClear;
        m_render_pass_descriptor.colorAttachments[1].storeAction = MTLStoreActionStore;
        m_render_pass_descriptor.colorAttachments[2].clearColor = MTLClearColorMake(0, 0, 0, 1);
        m_render_pass_descriptor.colorAttachments[2].loadAction = MTLLoadActionClear;
        m_render_pass_descriptor.colorAttachments[2].storeAction = MTLStoreActionStore;
        
        m_render_pass_descriptor.depthAttachment.texture = mtl_depth_stencil_attachment;
        m_render_pass_descriptor.depthAttachment.clearDepth = 1.0;
        m_render_pass_descriptor.depthAttachment.loadAction = MTLLoadActionClear;
        m_render_pass_descriptor.depthAttachment.storeAction = MTLStoreActionStore;
        
        m_render_pass_descriptor.stencilAttachment.texture = mtl_depth_stencil_attachment;
        m_render_pass_descriptor.stencilAttachment.clearStencil = 0;
        m_render_pass_descriptor.stencilAttachment.loadAction = MTLLoadActionClear;
        m_render_pass_descriptor.stencilAttachment.storeAction = MTLStoreActionStore;
    }
    
    mtl_render_pass_descriptor_impl::mtl_render_pass_descriptor_impl(const std::string& name, void* mtl_raw_color_attachment_ptr, void* mtl_raw_depth_stencil_attachment_ptr)
    {
        m_is_main_render_pass_descriptor = true;
        m_name = [NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding];
        m_render_pass_descriptor = [MTLRenderPassDescriptor new];
        
        id<MTLTexture> mtl_color_attachment = (__bridge id<MTLTexture>)mtl_raw_color_attachment_ptr;
        id<MTLTexture> mtl_depth_stencil_attachment = (__bridge id<MTLTexture>)mtl_raw_depth_stencil_attachment_ptr;
        
        m_color_attachments_pixel_format.push_back(mtl_device::get_instance()->get_color_pixel_format());
        
        m_render_pass_descriptor.colorAttachments[0].loadAction = MTLLoadActionClear;
        m_render_pass_descriptor.colorAttachments[0].clearColor = MTLClearColorMake(0, 0, 0, 1);
        m_render_pass_descriptor.colorAttachments[0].storeAction = MTLStoreActionStore;
        m_render_pass_descriptor.depthAttachment.loadAction = MTLLoadActionLoad;
        m_render_pass_descriptor.stencilAttachment.loadAction = MTLLoadActionLoad;
        
        m_render_pass_descriptor.colorAttachments[0].texture = mtl_color_attachment;
        m_render_pass_descriptor.depthAttachment.texture = mtl_depth_stencil_attachment;
        m_render_pass_descriptor.stencilAttachment.texture = mtl_depth_stencil_attachment;
    }
    
    mtl_render_pass_descriptor_impl::~mtl_render_pass_descriptor_impl()
    {

    }
    
    void* mtl_render_pass_descriptor_impl::get_mtl_render_pass_descriptor_ptr() const
    {
        return (__bridge void*)m_render_pass_descriptor;
    }
    
    void* mtl_render_pass_descriptor_impl::get_mtl_render_commnad_encoder() const
    {
        return (__bridge void*)m_render_command_encoder;
    }
    
    void* mtl_render_pass_descriptor_impl::get_mtl_render_encoder(const std::string& guid) const
    {
        return (__bridge void*)m_render_command_encoder;
    }
    
    bool mtl_render_pass_descriptor_impl::is_color_attachment_exist(i32 index) const
    {
        bool result = false;
        if (m_render_pass_descriptor.colorAttachments[index].texture)
        {
            result = true;
        }
        return result;
    }
    
    ui64 mtl_render_pass_descriptor_impl::get_color_attachment_pixel_format(i32 index) const
    {
        ui64 result = 0;
        if (index < m_color_attachments_pixel_format.size())
        {
            result = m_color_attachments_pixel_format.at(index);
        }
        else
        {
            assert(false);
        }
        return result;
    }
    
    std::vector<texture_shared_ptr> mtl_render_pass_descriptor_impl::get_color_attachments_texture()
    {
        return m_color_attachments_texture;
    }
    
    void mtl_render_pass_descriptor_impl::bind()
    {
        if (m_is_main_render_pass_descriptor)
        {
            id<MTLTexture> mtl_color_attachment = (__bridge id<MTLTexture>)mtl_device::get_instance()->get_mtl_raw_color_attachment_ptr();
            m_render_pass_descriptor.colorAttachments[0].texture = mtl_color_attachment;
        }
        
        id<MTLCommandBuffer> mtl_command_buffer = (__bridge id<MTLCommandBuffer>)mtl_device::get_instance()->get_mtl_raw_command_buffer_ptr();
        m_render_command_encoder = [mtl_command_buffer renderCommandEncoderWithDescriptor:m_render_pass_descriptor];
        m_render_command_encoder.label = m_name;
    }
    
    void mtl_render_pass_descriptor_impl::unbind()
    {
        [m_render_command_encoder endEncoding];
    }
    
    mtl_render_pass_descriptor::mtl_render_pass_descriptor(const std::string& name, ui32 width, ui32 height)
    {
        m_name = name;
        m_render_pass_descriptor_impl = std::make_shared<mtl_render_pass_descriptor_impl>(m_name, width, height);
    }
    
    mtl_render_pass_descriptor::mtl_render_pass_descriptor(const std::string& name, void* mtl_raw_color_attachment_ptr, void* mtl_raw_depth_stencil_attachment_ptr)
    {
        m_name = name;
        m_render_pass_descriptor_impl = std::make_shared<mtl_render_pass_descriptor_impl>(m_name, mtl_raw_color_attachment_ptr, mtl_raw_depth_stencil_attachment_ptr);
    }
    
    mtl_render_pass_descriptor::~mtl_render_pass_descriptor()
    {
        
    }
    
    void* mtl_render_pass_descriptor::get_mtl_render_pass_descriptor_ptr() const
    {
        return impl_as<mtl_render_pass_descriptor_impl>()->get_mtl_render_pass_descriptor_ptr();
    }
    
    void mtl_render_pass_descriptor::bind()
    {
        mtl_device::get_instance()->set_current_render_pass_descriptor(shared_from_this());
        impl_as<mtl_render_pass_descriptor_impl>()->bind();
    }
    
    void mtl_render_pass_descriptor::unbind()
    {
        impl_as<mtl_render_pass_descriptor_impl>()->unbind();
        mtl_device::get_instance()->set_current_render_pass_descriptor(nullptr);
    }
    
    void* mtl_render_pass_descriptor::get_mtl_render_commnad_encoder() const
    {
        return impl_as<mtl_render_pass_descriptor_impl>()->get_mtl_render_commnad_encoder();
    }
    
    void* mtl_render_pass_descriptor::get_mtl_render_encoder(const std::string& guid) const
    {
        return impl_as<mtl_render_pass_descriptor_impl>()->get_mtl_render_encoder(guid);
    }
    
    bool mtl_render_pass_descriptor::is_color_attachment_exist(i32 index) const
    {
        return impl_as<mtl_render_pass_descriptor_impl>()->is_color_attachment_exist(index);
    }
    
    ui64 mtl_render_pass_descriptor::get_color_attachment_pixel_format(i32 index) const
    {
        return impl_as<mtl_render_pass_descriptor_impl>()->get_color_attachment_pixel_format(index);
    }
    
    std::vector<texture_shared_ptr> mtl_render_pass_descriptor::get_color_attachments_texture()
    {
        return impl_as<mtl_render_pass_descriptor_impl>()->get_color_attachments_texture();
    }
}

#endif
