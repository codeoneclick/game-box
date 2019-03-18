//
//  mtl_render_pipeline_state.cpp
//  gbCore
//
//  Created by serhii.s on 3/6/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "mtl_render_pipeline_state.h"
#include "mtl_device.h"
#include "mtl_vertex_descriptor.h"
#include "mtl_render_pass_descriptor.h"

#if USED_GRAPHICS_API == METAL_API

#include <MetalKit/MetalKit.h>

namespace gb
{
    class mtl_render_pipeline_state_impl : public i_mtl_render_pipeline_state_impl
    {
    private:
        
        id<MTLRenderPipelineState> m_render_pipeline_state = nil;
        MTLRenderPipelineDescriptor* m_render_pipeline_state_descriptor = nil;
        
        id<MTLFunction> m_vertex_program = nil;
        id<MTLFunction> m_fragment_program = nil;
        
    protected:
        
    public:
        
        mtl_render_pipeline_state_impl(const std::string& program_name,
                                       const mtl_vertex_descriptor_shared_ptr& vertex_descriptor);
        ~mtl_render_pipeline_state_impl();
        
        void* get_mtl_raw_render_pipeline_state_ptr() const override;
    };
    
    mtl_render_pipeline_state_impl::mtl_render_pipeline_state_impl(const std::string& program_name,
                                                                   const mtl_vertex_descriptor_shared_ptr& vertex_descriptor)
    {
        const auto mtl_device_wrapper = gb::mtl_device::get_instance();
        id<MTLDevice> mtl_device_raw = (__bridge id<MTLDevice>)mtl_device_wrapper->get_mtl_raw_device_ptr();
        id<MTLLibrary> mtl_library_raw =  (__bridge id<MTLLibrary>)mtl_device_wrapper->get_mtl_raw_library_ptr();
        const auto mtl_render_pass_descriptor_wrapper = mtl_device_wrapper->get_current_render_pass_descriptor();
        MTLRenderPassDescriptor* mtl_render_pass_descriptor = (__bridge MTLRenderPassDescriptor*)mtl_render_pass_descriptor_wrapper->get_mtl_render_pass_descriptor_ptr();
        MTLVertexDescriptor* mtl_vertex_descriptor = (__bridge MTLVertexDescriptor*)vertex_descriptor->get_mtl_vertex_descriptor_ptr();
        
        std::string vertex_program_name = "vertex_";
        vertex_program_name.append(program_name);
        std::string fragment_program_name = "fragment_";
        fragment_program_name.append(program_name);
        m_vertex_program = [mtl_library_raw newFunctionWithName:[NSString stringWithCString:vertex_program_name.c_str() encoding:NSUTF8StringEncoding]];
        m_fragment_program = [mtl_library_raw newFunctionWithName:[NSString stringWithCString:fragment_program_name.c_str() encoding:NSUTF8StringEncoding]];
        
        m_render_pipeline_state_descriptor = [[MTLRenderPipelineDescriptor alloc] init];
        
        [m_render_pipeline_state_descriptor setSampleCount:mtl_device_wrapper->get_samples_count()];
        [m_render_pipeline_state_descriptor setVertexFunction:m_vertex_program];
        [m_render_pipeline_state_descriptor setFragmentFunction:m_fragment_program];
        [m_render_pipeline_state_descriptor setVertexDescriptor:mtl_vertex_descriptor];
        
        if (mtl_render_pass_descriptor_wrapper->is_color_attachment_exist(0))
        {
            MTLPixelFormat color_attachment_pixel_format = static_cast<MTLPixelFormat>(mtl_render_pass_descriptor_wrapper->get_color_attachment_pixel_format(0));
            m_render_pipeline_state_descriptor.colorAttachments[0].pixelFormat = color_attachment_pixel_format;
        }
        
        if (mtl_render_pass_descriptor_wrapper->is_color_attachment_exist(1))
        {
            MTLPixelFormat color_attachment_pixel_format = static_cast<MTLPixelFormat>(mtl_render_pass_descriptor_wrapper->get_color_attachment_pixel_format(1));
            m_render_pipeline_state_descriptor.colorAttachments[1].pixelFormat = color_attachment_pixel_format;
        }
        
        if (mtl_render_pass_descriptor_wrapper->is_color_attachment_exist(2))
        {
            MTLPixelFormat color_attachment_pixel_format = static_cast<MTLPixelFormat>(mtl_render_pass_descriptor_wrapper->get_color_attachment_pixel_format(2));
            m_render_pipeline_state_descriptor.colorAttachments[2].pixelFormat = color_attachment_pixel_format;
        }
        
        MTLPixelFormat depth_stencil_pixel_format = mtl_render_pass_descriptor.depthAttachment.texture.pixelFormat;
        m_render_pipeline_state_descriptor.depthAttachmentPixelFormat = depth_stencil_pixel_format;
        m_render_pipeline_state_descriptor.stencilAttachmentPixelFormat = depth_stencil_pixel_format;
        
        NSError* error = nil;
        m_render_pipeline_state = [mtl_device_raw newRenderPipelineStateWithDescriptor:m_render_pipeline_state_descriptor
                                                                                 error:&error];
        if (!m_render_pipeline_state_descriptor)
        {
            NSLog(@"failed to created render pipeline state, error %@", error);
        }
    }
    
    mtl_render_pipeline_state_impl::~mtl_render_pipeline_state_impl()
    {
        
    }
    
    void* mtl_render_pipeline_state_impl::get_mtl_raw_render_pipeline_state_ptr() const
    {
        return (__bridge void*)m_render_pipeline_state;
    }
    
    mtl_render_pipeline_state::mtl_render_pipeline_state(const std::string& program_name,
                                                         const mtl_vertex_descriptor_shared_ptr& vertex_descriptor)
    {
        m_mtl_render_pipeline_state_impl = std::make_shared<mtl_render_pipeline_state_impl>(program_name,
                                                                                            vertex_descriptor);
    }
    
    mtl_render_pipeline_state::~mtl_render_pipeline_state()
    {
        
    }
    
    void* mtl_render_pipeline_state::get_mtl_raw_render_pipeline_state_ptr() const
    {
        return impl_as<mtl_render_pipeline_state_impl>()->get_mtl_raw_render_pipeline_state_ptr();
    }
}

#endif
