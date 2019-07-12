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

@interface mtl_render_command_encoder_wrapper : NSObject

@property(nonatomic, readonly, nonnull) id<MTLRenderCommandEncoder> m_render_command_encoder;
@property(nonatomic, readonly, nonnull) MTLRenderPassDescriptor* m_render_pass_descriptor;
@property(nonatomic, readonly) BOOL m_should_reconstruct;

- (void)bind;
- (void)unbind;
- (void)reconstruct:(id<MTLCommandBuffer>) command_buffer;
- (void)reconstruct:(id<MTLCommandBuffer>) command_buffer with_custom_render_pass_descriptor:(MTLRenderPassDescriptor *) render_pass_descriptor;

@end

@implementation mtl_render_command_encoder_wrapper

- (instancetype)init
{
    self = [super init];
    if(self)
    {
        _m_render_pass_descriptor = [MTLRenderPassDescriptor renderPassDescriptor];
        _m_should_reconstruct = YES;
        
    }
    return self;
}

- (void)bind
{
    
}

- (void)unbind
{
    _m_should_reconstruct = YES;
    [_m_render_command_encoder endEncoding];
}

- (void)reconstruct:(id<MTLCommandBuffer>) command_buffer
{
    _m_render_command_encoder = [command_buffer renderCommandEncoderWithDescriptor:_m_render_pass_descriptor];
    _m_should_reconstruct = NO;
}

- (void)reconstruct:(id<MTLCommandBuffer>) command_buffer with_custom_render_pass_descriptor:(MTLRenderPassDescriptor *) render_pass_descriptor
{
    _m_render_command_encoder = [command_buffer renderCommandEncoderWithDescriptor:render_pass_descriptor];
    _m_render_pass_descriptor = render_pass_descriptor;
    _m_should_reconstruct = NO;
}

@end

namespace gb
{
    class mtl_render_pass_descriptor_impl : public i_mtl_render_pass_descriptor_impl
    {
    private:
        
        enum e_render_pass_descriptor_mode
        {
            e_undedfined,
            e_ws,
            e_ss,
            e_output
        };
        
        NSString* m_name;
        mtl_render_command_encoder_wrapper* m_render_command_encoder_wrapper = nil;

        std::vector<ui64> m_color_attachments_pixel_format;
        std::vector<texture_shared_ptr> m_color_attachments_texture;
        
        e_render_pass_descriptor_mode m_mode = e_render_pass_descriptor_mode::e_undedfined;
        
        f32 m_frame_width = 0.f;
        f32 m_frame_height = 0.f;
        
        protected:
        
        void add_attachments(const std::string& guid,
                             ui32 frame_width,
                             ui32 frame_height,
                             const std::vector<std::shared_ptr<configuration>>& attachments_configurations);
        
        public:
        
        ~mtl_render_pass_descriptor_impl();
        
        static std::shared_ptr<mtl_render_pass_descriptor_impl> construct_ws_render_pass_descriptor(const std::shared_ptr<ws_technique_configuration>& configuration);
        static std::shared_ptr<mtl_render_pass_descriptor_impl> construct_ss_render_pass_descriptor(const std::shared_ptr<ss_technique_configuration>& configuration);
        static std::shared_ptr<mtl_render_pass_descriptor_impl> construct_output_render_pass_descriptor(const std::string& name, void* mtl_raw_color_attachment_ptr, void* mtl_raw_depth_stencil_attachment_ptr);
        
        void* get_mtl_render_pass_descriptor_ptr() const override;
        void* get_mtl_render_commnad_encoder() const override;
        void* get_mtl_render_encoder(const std::string& guid) const override;
        
        ui32 get_color_attachments_num() const override;
        bool is_color_attachment_exist(i32 index) const override;
        ui64 get_color_attachment_pixel_format(i32 index) const override;
        ui64 get_depth_stencil_attachment_pixel_format() const override;
        
        std::vector<texture_shared_ptr> get_color_attachments_texture() override;
        
        void bind() override;
        void unbind() override;
    };
    
    std::shared_ptr<mtl_render_pass_descriptor_impl> mtl_render_pass_descriptor_impl::construct_ws_render_pass_descriptor(const std::shared_ptr<ws_technique_configuration>& configuration)
    {
        const auto render_pass_descriptor = std::make_shared<mtl_render_pass_descriptor_impl>();
        render_pass_descriptor->m_mode = e_render_pass_descriptor_mode::e_ws;
        render_pass_descriptor->m_name = [NSString stringWithCString:configuration->get_guid().c_str() encoding:NSUTF8StringEncoding];
        render_pass_descriptor->m_render_command_encoder_wrapper = [mtl_render_command_encoder_wrapper new];
        
        render_pass_descriptor->m_frame_width = configuration->get_frame_width();
        render_pass_descriptor->m_frame_height = configuration->get_frame_height();
        
        id<MTLTexture> mtl_depth_stencil_attachment = (__bridge id<MTLTexture>)gb::mtl_device::get_instance()->get_mtl_raw_depth_stencil_attachment_ptr();
        
        const auto attachments_configurations = configuration->get_attachments_configurations();
        assert(attachments_configurations.size() != 0);
        
        if (attachments_configurations.size() != 0)
        {
            render_pass_descriptor->add_attachments(configuration->get_guid(),
                                                    configuration->get_frame_width(),
                                                    configuration->get_frame_height(),
                                                    attachments_configurations);
        }
        
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.depthAttachment.texture = mtl_depth_stencil_attachment;
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.depthAttachment.clearDepth = 1.0;
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.depthAttachment.loadAction = configuration->get_is_depth_compare_mode_enabled() ? MTLLoadActionLoad : MTLLoadActionClear;
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.depthAttachment.storeAction = MTLStoreActionStore;
        
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.stencilAttachment.texture = mtl_depth_stencil_attachment;
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.stencilAttachment.clearStencil = 0;
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.stencilAttachment.loadAction = configuration->get_is_depth_compare_mode_enabled() ? MTLLoadActionLoad : MTLLoadActionClear;
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.stencilAttachment.storeAction = MTLStoreActionStore;
        
        return render_pass_descriptor;
    }
    
    std::shared_ptr<mtl_render_pass_descriptor_impl> mtl_render_pass_descriptor_impl::construct_ss_render_pass_descriptor(const std::shared_ptr<ss_technique_configuration>& configuration)
    {
        const auto render_pass_descriptor = std::make_shared<mtl_render_pass_descriptor_impl>();
        render_pass_descriptor->m_mode = e_render_pass_descriptor_mode::e_ss;
        render_pass_descriptor->m_name = [NSString stringWithCString:configuration->get_guid().c_str() encoding:NSUTF8StringEncoding];
        render_pass_descriptor->m_render_command_encoder_wrapper = [mtl_render_command_encoder_wrapper new];
        
        id<MTLTexture> mtl_depth_stencil_attachment = (__bridge id<MTLTexture>)gb::mtl_device::get_instance()->get_mtl_raw_depth_stencil_attachment_ptr();
        
        const auto attachments_configurations = configuration->get_attachments_configurations();
        assert(attachments_configurations.size() != 0);
        
        if (attachments_configurations.size() != 0)
        {
            render_pass_descriptor->add_attachments(configuration->get_guid(),
                                                    configuration->get_frame_width(),
                                                    configuration->get_frame_height(),
                                                    attachments_configurations);
        }
        
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.depthAttachment.texture = mtl_depth_stencil_attachment;
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.depthAttachment.clearDepth = 1.0;
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.depthAttachment.loadAction = MTLLoadActionClear;
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.depthAttachment.storeAction = MTLStoreActionStore;
        
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.stencilAttachment.texture = mtl_depth_stencil_attachment;
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.stencilAttachment.clearStencil = 0;
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.stencilAttachment.loadAction = MTLLoadActionClear;
        render_pass_descriptor->m_render_command_encoder_wrapper.m_render_pass_descriptor.stencilAttachment.storeAction = MTLStoreActionStore;
        
        return render_pass_descriptor;
    }
    
     std::shared_ptr<mtl_render_pass_descriptor_impl> mtl_render_pass_descriptor_impl::construct_output_render_pass_descriptor(const std::string& name, void* mtl_raw_color_attachment_ptr, void* mtl_raw_depth_stencil_attachment_ptr)
    {
        const auto render_pass_descriptor = std::make_shared<mtl_render_pass_descriptor_impl>();
        render_pass_descriptor->m_mode = e_render_pass_descriptor_mode::e_output;
        render_pass_descriptor->m_render_command_encoder_wrapper = [mtl_render_command_encoder_wrapper new];
        render_pass_descriptor->m_name = [NSString stringWithCString:name.c_str() encoding:NSUTF8StringEncoding];
        render_pass_descriptor->m_color_attachments_pixel_format.push_back(mtl_device::get_instance()->get_color_pixel_format());
        
        auto attachment_texture = gb::texture::construct(name, nullptr, 0, 0);
        render_pass_descriptor->m_color_attachments_texture.push_back(attachment_texture);
        
        return render_pass_descriptor;
    }
    
    mtl_render_pass_descriptor_impl::~mtl_render_pass_descriptor_impl()
    {
        
    }
    
    void mtl_render_pass_descriptor_impl::add_attachments(const std::string& guid,
                                                          ui32 frame_width,
                                                          ui32 frame_height,
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
            multisample_attachment_texture_descriptor.sampleCount = static_cast<NSUInteger>(samples_count);
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
                multisample_texture = gb::texture::construct(mtl_texture_guid, mtl_texture_wrapper, frame_width, frame_height);
                m_color_attachments_texture.push_back(multisample_texture);
            }
            else
            {
                m_color_attachments_texture.push_back(texture);
            }
            
            m_color_attachments_pixel_format.push_back(pixel_format);
            m_render_command_encoder_wrapper.m_render_pass_descriptor.colorAttachments[attachment_index].texture = is_multisample ? mtl_raw_multisample_texture : mtl_raw_texture;
            m_render_command_encoder_wrapper.m_render_pass_descriptor.colorAttachments[attachment_index].resolveTexture = is_multisample ? mtl_raw_texture : nil;
            
            m_render_command_encoder_wrapper.m_render_pass_descriptor.colorAttachments[attachment_index].clearColor = MTLClearColorMake(attachment_configuration->get_clear_color_r(),
                                                                                                       attachment_configuration->get_clear_color_g(),
                                                                                                       attachment_configuration->get_clear_color_b(),
                                                                                                       attachment_configuration->get_clear_color_a());
            m_render_command_encoder_wrapper.m_render_pass_descriptor.colorAttachments[attachment_index].loadAction = MTLLoadActionClear;
            m_render_command_encoder_wrapper.m_render_pass_descriptor.colorAttachments[attachment_index].storeAction = is_multisample ? MTLStoreActionStoreAndMultisampleResolve : MTLStoreActionStore;
            
            attachment_index++;
        }
    }
    
    void* mtl_render_pass_descriptor_impl::get_mtl_render_pass_descriptor_ptr() const
    {
        return (__bridge void*)m_render_command_encoder_wrapper.m_render_pass_descriptor;
    }
    
    void* mtl_render_pass_descriptor_impl::get_mtl_render_commnad_encoder() const
    {
        return (__bridge void*)m_render_command_encoder_wrapper.m_render_command_encoder;
    }
    
    void* mtl_render_pass_descriptor_impl::get_mtl_render_encoder(const std::string& guid) const
    {
        return (__bridge void*)m_render_command_encoder_wrapper.m_render_command_encoder;
    }
    
    ui32 mtl_render_pass_descriptor_impl::get_color_attachments_num() const
    {
        return static_cast<ui32>(m_color_attachments_texture.size());
    }
    
    bool mtl_render_pass_descriptor_impl::is_color_attachment_exist(i32 index) const
    {
        bool result = false;
        if (m_render_command_encoder_wrapper.m_render_pass_descriptor.colorAttachments[index].texture)
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
        if (m_mode != e_render_pass_descriptor_mode::e_output)
        {
            return m_render_command_encoder_wrapper.m_render_pass_descriptor.depthAttachment.texture.pixelFormat;
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
        if (m_mode == e_render_pass_descriptor_mode::e_output)
        {
            [m_render_command_encoder_wrapper reconstruct:(__bridge id<MTLCommandBuffer>)mtl_device::get_instance()->get_mtl_raw_output_command_buffer_ptr() with_custom_render_pass_descriptor:(__bridge MTLRenderPassDescriptor *)mtl_device::get_instance()->get_mtl_raw_current_render_pass_descriptor()];
        }
        else if (m_mode == e_render_pass_descriptor_mode::e_ws)
        {
            [m_render_command_encoder_wrapper reconstruct:(__bridge id<MTLCommandBuffer>)mtl_device::get_instance()->get_mtl_raw_ws_command_buffer_ptr()];
            m_render_command_encoder_wrapper.m_render_command_encoder.label = m_name;
        }
        else if (m_mode == e_render_pass_descriptor_mode::e_ss)
        {
            [m_render_command_encoder_wrapper reconstruct:(__bridge id<MTLCommandBuffer>)mtl_device::get_instance()->get_mtl_raw_ss_command_buffer_ptr()];
            m_render_command_encoder_wrapper.m_render_command_encoder.label = m_name;
        }
        else
        {
            assert(false);
        }
        [m_render_command_encoder_wrapper bind];
    }
    
    void mtl_render_pass_descriptor_impl::unbind()
    {
        [m_render_command_encoder_wrapper unbind];
    }
    
    std::shared_ptr<mtl_render_pass_descriptor> mtl_render_pass_descriptor::construct_ws_render_pass_descriptor(const std::shared_ptr<ws_technique_configuration>& configuration)
    {
        std::shared_ptr<mtl_render_pass_descriptor> render_pass_descriptor = std::make_shared<mtl_render_pass_descriptor>();
        render_pass_descriptor->m_render_pass_descriptor_impl = mtl_render_pass_descriptor_impl::construct_ws_render_pass_descriptor(configuration);
        return render_pass_descriptor;
    }
    
    std::shared_ptr<mtl_render_pass_descriptor> mtl_render_pass_descriptor::construct_ss_render_pass_descriptor(const std::shared_ptr<ss_technique_configuration>& configuration)
    {
        std::shared_ptr<mtl_render_pass_descriptor> render_pass_descriptor = std::make_shared<mtl_render_pass_descriptor>();
        render_pass_descriptor->m_render_pass_descriptor_impl = mtl_render_pass_descriptor_impl::construct_ss_render_pass_descriptor(configuration);
        return render_pass_descriptor;
    }
    
    std::shared_ptr<mtl_render_pass_descriptor> mtl_render_pass_descriptor::construct_output_render_pass_descriptor(const std::string& name, void* mtl_raw_color_attachment_ptr, void* mtl_raw_depth_stencil_attachment_ptr)
    {
        std::shared_ptr<mtl_render_pass_descriptor> render_pass_descriptor = std::make_shared<mtl_render_pass_descriptor>();
        render_pass_descriptor->m_render_pass_descriptor_impl = mtl_render_pass_descriptor_impl::construct_output_render_pass_descriptor(name, mtl_raw_color_attachment_ptr, mtl_raw_depth_stencil_attachment_ptr);
        return render_pass_descriptor;
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
    
    ui32 mtl_render_pass_descriptor::get_color_attachments_num() const
    {
         return impl_as<mtl_render_pass_descriptor_impl>()->get_color_attachments_num();
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
