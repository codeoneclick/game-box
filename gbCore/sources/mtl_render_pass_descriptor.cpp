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
#include "attachment_configuration.h"

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
        
        void construct_attachments(const std::string& guid,
                                   ui32 frame_width,
                                   ui32 frame_height,
                                   const glm::vec4& clear_color,
                                   const std::vector<std::shared_ptr<configuration>>& attachments_configurations);
        
    public:
        
        mtl_render_pass_descriptor_impl(const std::shared_ptr<ws_technique_configuration>& configuration);
        mtl_render_pass_descriptor_impl(const std::shared_ptr<ss_technique_configuration>& configuration);
        mtl_render_pass_descriptor_impl(const std::string& name, void* mtl_raw_color_attachment_ptr, void* mtl_raw_depth_stencil_attachment_ptr);
        ~mtl_render_pass_descriptor_impl();
        
        void* get_mtl_render_pass_descriptor_ptr() const override;
        void* get_mtl_render_commnad_encoder() const override;
        void* get_mtl_render_encoder(const std::string& guid) const override;
        
        bool is_color_attachment_exist(i32 index) const override;
        ui64 get_color_attachment_pixel_format(i32 index) const override;
        ui64 get_depth_stencil_attachment_pixel_format() const override;
        
        std::vector<texture_shared_ptr> get_color_attachments_texture() override;
        
        void bind() override;
        void unbind() override;
    };
    
    mtl_render_pass_descriptor_impl::mtl_render_pass_descriptor_impl(const std::shared_ptr<ws_technique_configuration>& configuration)
    {
        m_name = [NSString stringWithCString:configuration->get_guid().c_str() encoding:NSUTF8StringEncoding];
        m_render_pass_descriptor = [MTLRenderPassDescriptor new];
        
        id<MTLTexture> mtl_depth_stencil_attachment = (__bridge id<MTLTexture>)gb::mtl_device::get_instance()->get_mtl_raw_depth_stencil_attachment_ptr();
        
        const auto attachments_configurations = configuration->get_attachments_configurations();
        assert(attachments_configurations.size() != 0);
        
        if (attachments_configurations.size() != 0)
        {
            const auto clear_color = glm::vec4(configuration->get_clear_color_r(),
                                               configuration->get_clear_color_g(),
                                               configuration->get_clear_color_b(),
                                               configuration->get_clear_color_a());
            construct_attachments(configuration->get_guid(),
                                  configuration->get_screen_width(),
                                  configuration->get_screen_height(),
                                  clear_color,
                                  attachments_configurations);
        }
        
        m_render_pass_descriptor.depthAttachment.texture = mtl_depth_stencil_attachment;
        m_render_pass_descriptor.depthAttachment.clearDepth = 1.0;
        m_render_pass_descriptor.depthAttachment.loadAction = MTLLoadActionLoad;
        m_render_pass_descriptor.depthAttachment.storeAction = MTLStoreActionStore;
        
        m_render_pass_descriptor.stencilAttachment.texture = mtl_depth_stencil_attachment;
        m_render_pass_descriptor.stencilAttachment.clearStencil = 0;
        m_render_pass_descriptor.stencilAttachment.loadAction = MTLLoadActionLoad;
        m_render_pass_descriptor.stencilAttachment.storeAction = MTLStoreActionStore;
    }
    
    mtl_render_pass_descriptor_impl::mtl_render_pass_descriptor_impl(const std::shared_ptr<ss_technique_configuration>& configuration)
    {
        m_name = [NSString stringWithCString:configuration->get_guid().c_str() encoding:NSUTF8StringEncoding];
        m_render_pass_descriptor = [MTLRenderPassDescriptor new];
        
        id<MTLTexture> mtl_depth_stencil_attachment = (__bridge id<MTLTexture>)gb::mtl_device::get_instance()->get_mtl_raw_depth_stencil_attachment_ptr();
        
        const auto attachments_configurations = configuration->get_attachments_configurations();
        assert(attachments_configurations.size() != 0);
        
        if (attachments_configurations.size() != 0)
        {
            const auto clear_color = glm::vec4(0.f,
                                               0.f,
                                               0.f,
                                               1.f);
            construct_attachments(configuration->get_guid(),
                                  configuration->get_screen_width(),
                                  configuration->get_screen_height(),
                                  clear_color,
                                  attachments_configurations);
        }
        
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
        m_color_attachments_pixel_format.push_back(mtl_device::get_instance()->get_color_pixel_format());
    }
    
    mtl_render_pass_descriptor_impl::~mtl_render_pass_descriptor_impl()
    {

    }
    
    void mtl_render_pass_descriptor_impl::construct_attachments(const std::string& guid,
                                                                ui32 frame_width,
                                                                ui32 frame_height,
                                                                const glm::vec4& clear_color,
                                                                const std::vector<std::shared_ptr<configuration>>& attachments_configurations)
    {
        MTLTextureDescriptor *attachment_texture_descriptor =
        [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm_sRGB
                                                           width:frame_width
                                                          height:frame_height
                                                       mipmapped:NO];
        attachment_texture_descriptor.sampleCount = 1;
        attachment_texture_descriptor.textureType = MTLTextureType2D;
        attachment_texture_descriptor.usage |= MTLTextureUsageRenderTarget;
        attachment_texture_descriptor.storageMode = MTLStorageModePrivate;
        
        const auto mtl_device_wrapper = gb::mtl_device::get_instance();
        ui64 samples_count = mtl_device_wrapper->get_samples_count();
        bool is_multisample = samples_count > 1;
        MTLTextureDescriptor *multisample_attachment_texture_descriptor = nil;
        
        if (is_multisample)
        {
            multisample_attachment_texture_descriptor =
            [MTLTextureDescriptor texture2DDescriptorWithPixelFormat:MTLPixelFormatRGBA8Unorm_sRGB
                                                               width:frame_width
                                                              height:frame_height
                                                           mipmapped:NO];
            multisample_attachment_texture_descriptor.sampleCount = samples_count;
            multisample_attachment_texture_descriptor.textureType = MTLTextureType2DMultisample;
            multisample_attachment_texture_descriptor.usage |= MTLTextureUsageRenderTarget;
            multisample_attachment_texture_descriptor.storageMode = MTLStorageModePrivate;
        }
        
        i32 attachment_index = 0;
        for (auto attachment_configuration_it : attachments_configurations)
        {
            const auto attachment_configuration = std::static_pointer_cast<gb::attachment_configuration>(attachment_configuration_it);
            
            MTLPixelFormat pixel_format = static_cast<MTLPixelFormat>(attachment_configuration->get_pixel_format());
            attachment_texture_descriptor.pixelFormat = pixel_format;
            
            if (is_multisample)
            {
                multisample_attachment_texture_descriptor.pixelFormat = pixel_format;
            }
            
            auto mtl_texture_wrapper = std::make_shared<mtl_texture>((__bridge void*)attachment_texture_descriptor);
            id<MTLTexture> mtl_raw_texture = (__bridge id<MTLTexture>)mtl_texture_wrapper->get_mtl_raw_texture_ptr();
            
            std::shared_ptr<mtl_texture> mtl_multisample_texture_wrapper = nullptr;
            id<MTLTexture> mtl_raw_multisample_texture = nil;
            
            if (is_multisample)
            {
                mtl_multisample_texture_wrapper = std::make_shared<mtl_texture>((__bridge void*)multisample_attachment_texture_descriptor);
                mtl_raw_multisample_texture = (__bridge id<MTLTexture>)mtl_multisample_texture_wrapper->get_mtl_raw_texture_ptr();
            }
            
            std::string mtl_texture_guid = guid;
            mtl_texture_guid.append(".");
            mtl_texture_guid.append(attachment_configuration->get_name());
            mtl_raw_texture.label = [NSString stringWithCString:mtl_texture_guid.c_str() encoding:NSUTF8StringEncoding];
           
            auto texture = gb::texture::construct(mtl_texture_guid, mtl_texture_wrapper, frame_width, frame_height);
            
            std::shared_ptr<gb::texture> multisample_texture = nullptr;
            if (is_multisample)
            {
                multisample_texture = gb::texture::construct(mtl_texture_guid, mtl_multisample_texture_wrapper, frame_width, frame_height);
            }
            
            m_color_attachments_texture.push_back(texture);
            m_color_attachments_pixel_format.push_back(pixel_format);
            m_render_pass_descriptor.colorAttachments[attachment_index].texture = is_multisample ? mtl_raw_multisample_texture : mtl_raw_texture;
            m_render_pass_descriptor.colorAttachments[attachment_index].resolveTexture = is_multisample ? mtl_raw_texture : nil;
            
            m_render_pass_descriptor.colorAttachments[attachment_index].clearColor = MTLClearColorMake(clear_color.x,
                                                                                                       clear_color.y,
                                                                                                       clear_color.z,
                                                                                                       clear_color.w);
            m_render_pass_descriptor.colorAttachments[attachment_index].loadAction = MTLLoadActionClear;
            m_render_pass_descriptor.colorAttachments[attachment_index].storeAction = is_multisample ? MTLStoreActionStoreAndMultisampleResolve : MTLStoreActionStore;
            
            attachment_index++;
        }
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
        if (!result && index == 0 && m_is_main_render_pass_descriptor)
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
    
    ui64 mtl_render_pass_descriptor_impl::get_depth_stencil_attachment_pixel_format() const
    {
        if (!m_is_main_render_pass_descriptor)
        {
            return m_render_pass_descriptor.depthAttachment.texture.pixelFormat;
        }
        else
        {
            id<MTLTexture> mtl_depth_stencil_texture = (__bridge id<MTLTexture>)gb::mtl_device::get_instance()->get_mtl_raw_depth_stencil_attachment_ptr();
            return mtl_depth_stencil_texture.pixelFormat;
        }
    }
    
    std::vector<texture_shared_ptr> mtl_render_pass_descriptor_impl::get_color_attachments_texture()
    {
        return m_color_attachments_texture;
    }
    
    void mtl_render_pass_descriptor_impl::bind()
    {
        if (m_is_main_render_pass_descriptor)
        {
            m_render_command_encoder = (__bridge id<MTLRenderCommandEncoder>)mtl_device::get_instance()->get_mtl_raw_main_render_encoder();
        }
        else
        {
            id<MTLCommandBuffer> mtl_command_buffer = (__bridge id<MTLCommandBuffer>)mtl_device::get_instance()->get_mtl_raw_command_buffer_ptr();
            m_render_command_encoder = [mtl_command_buffer renderCommandEncoderWithDescriptor:m_render_pass_descriptor];
            m_render_command_encoder.label = m_name;
        }
    }
    
    void mtl_render_pass_descriptor_impl::unbind()
    {
        [m_render_command_encoder endEncoding];
    }
    
    mtl_render_pass_descriptor::mtl_render_pass_descriptor(const std::shared_ptr<ws_technique_configuration>& configuration)
    {
        m_name = configuration->get_guid();
        m_render_pass_descriptor_impl = std::make_shared<mtl_render_pass_descriptor_impl>(configuration);
    }
    
    mtl_render_pass_descriptor::mtl_render_pass_descriptor(const std::shared_ptr<ss_technique_configuration>& configuration)
    {
        m_name = configuration->get_guid();
        m_render_pass_descriptor_impl = std::make_shared<mtl_render_pass_descriptor_impl>(configuration);
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
    
    ui64 mtl_render_pass_descriptor::get_depth_stencil_attachment_pixel_format() const
    {
        return impl_as<mtl_render_pass_descriptor_impl>()->get_depth_stencil_attachment_pixel_format();
    }
    
    std::vector<texture_shared_ptr> mtl_render_pass_descriptor::get_color_attachments_texture()
    {
        return impl_as<mtl_render_pass_descriptor_impl>()->get_color_attachments_texture();
    }
}

#endif
