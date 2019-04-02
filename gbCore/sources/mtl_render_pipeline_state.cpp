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
#include "shader.h"

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
        
        MTLBlendFactor convert_blend_factor_from_gl_to_mtl(ui32 value);
        MTLBlendOperation convert_blend_operation_from_gl_to_mtl(ui32 value);
        
    public:
        
        mtl_render_pipeline_state_impl(const std::shared_ptr<material_cached_parameters>& material_parameters,
                                       const mtl_vertex_descriptor_shared_ptr& vertex_descriptor);
        ~mtl_render_pipeline_state_impl();
        
        void* get_mtl_raw_render_pipeline_state_ptr() const override;
    };
    
    mtl_render_pipeline_state_impl::mtl_render_pipeline_state_impl(const std::shared_ptr<material_cached_parameters>& material_parameters,
                                                                   const mtl_vertex_descriptor_shared_ptr& vertex_descriptor)
    {
        const auto mtl_device_wrapper = gb::mtl_device::get_instance();
        id<MTLDevice> mtl_device_raw = (__bridge id<MTLDevice>)mtl_device_wrapper->get_mtl_raw_device_ptr();
        id<MTLLibrary> mtl_library_raw =  (__bridge id<MTLLibrary>)mtl_device_wrapper->get_mtl_raw_library_ptr();
        const auto mtl_render_pass_descriptor_wrapper = mtl_device_wrapper->get_current_render_pass_descriptor();
        MTLVertexDescriptor* mtl_vertex_descriptor = (__bridge MTLVertexDescriptor*)vertex_descriptor->get_mtl_vertex_descriptor_ptr();
        
        std::string vertex_program_name = "vertex_";
        vertex_program_name.append(material_parameters->m_shader->get_guid());
        std::string fragment_program_name = "fragment_";
        fragment_program_name.append(material_parameters->m_shader->get_guid());
        m_vertex_program = [mtl_library_raw newFunctionWithName:[NSString stringWithCString:vertex_program_name.c_str() encoding:NSUTF8StringEncoding]];
        m_fragment_program = [mtl_library_raw newFunctionWithName:[NSString stringWithCString:fragment_program_name.c_str() encoding:NSUTF8StringEncoding]];
        
        m_render_pipeline_state_descriptor = [[MTLRenderPipelineDescriptor alloc] init];
        
        [m_render_pipeline_state_descriptor setRasterSampleCount:mtl_device_wrapper->get_samples_count()];
        [m_render_pipeline_state_descriptor setSampleCount:mtl_device_wrapper->get_samples_count()];

        [m_render_pipeline_state_descriptor setVertexFunction:m_vertex_program];
        [m_render_pipeline_state_descriptor setFragmentFunction:m_fragment_program];
        [m_render_pipeline_state_descriptor setVertexDescriptor:mtl_vertex_descriptor];
        
        //[m_render_pipeline_state_descriptor setAlphaToCoverageEnabled:material_parameters->m_is_depth_mask ? NO : YES];
        //[m_render_pipeline_state_descriptor setAlphaToOneEnabled:YES];
        
        ui32 attachments_num = mtl_render_pass_descriptor_wrapper->get_color_attachments_num();
        for (ui32 i = 0; i < attachments_num; ++i)
        {
            MTLPixelFormat color_attachment_pixel_format = static_cast<MTLPixelFormat>(mtl_render_pass_descriptor_wrapper->get_color_attachment_pixel_format(i));
            m_render_pipeline_state_descriptor.colorAttachments[i].pixelFormat = color_attachment_pixel_format;
            
            const auto blending_parameters = i < material_parameters->m_blending_parameters.size() ? material_parameters->m_blending_parameters.at(i) :
            material_parameters->m_blending_parameters.at(0);
            m_render_pipeline_state_descriptor.colorAttachments[i].blendingEnabled =blending_parameters->m_is_blending;
            m_render_pipeline_state_descriptor.colorAttachments[i].rgbBlendOperation = convert_blend_operation_from_gl_to_mtl(blending_parameters->m_blending_equation);
            m_render_pipeline_state_descriptor.colorAttachments[i].alphaBlendOperation = convert_blend_operation_from_gl_to_mtl(blending_parameters->m_blending_equation);
            m_render_pipeline_state_descriptor.colorAttachments[i].sourceRGBBlendFactor = convert_blend_factor_from_gl_to_mtl(blending_parameters->m_blending_function_source);
            m_render_pipeline_state_descriptor.colorAttachments[i].sourceAlphaBlendFactor = convert_blend_factor_from_gl_to_mtl(blending_parameters->m_blending_function_source);
            m_render_pipeline_state_descriptor.colorAttachments[i].destinationRGBBlendFactor = convert_blend_factor_from_gl_to_mtl(blending_parameters->m_blending_function_destination);
            m_render_pipeline_state_descriptor.colorAttachments[i].destinationAlphaBlendFactor = convert_blend_factor_from_gl_to_mtl(blending_parameters->m_blending_function_destination);
        }
        
        MTLPixelFormat depth_stencil_pixel_format = static_cast<MTLPixelFormat>(mtl_render_pass_descriptor_wrapper->get_depth_stencil_attachment_pixel_format());
        m_render_pipeline_state_descriptor.depthAttachmentPixelFormat = depth_stencil_pixel_format;
        m_render_pipeline_state_descriptor.stencilAttachmentPixelFormat = depth_stencil_pixel_format;
        
        NSError* error = nil;
        m_render_pipeline_state = [mtl_device_raw newRenderPipelineStateWithDescriptor:m_render_pipeline_state_descriptor
                                                                                 error:&error];
        if (!m_render_pipeline_state_descriptor || !m_render_pipeline_state)
        {
            NSLog(@"failed to created render pipeline state, error %@", error);
            assert(false);
        }
    }
    
    mtl_render_pipeline_state_impl::~mtl_render_pipeline_state_impl()
    {
        
    }
    
    MTLBlendFactor mtl_render_pipeline_state_impl::convert_blend_factor_from_gl_to_mtl(ui32 value)
    {
        if (value == gl::constant::zero)
        {
            return MTLBlendFactorZero;
        }
        
        if (value == gl::constant::one)
        {
            return MTLBlendFactorOne;
        }
        
        if (value == gl::constant::src_alpha)
        {
            return MTLBlendFactorSourceAlpha;
        }
        
        if (value == gl::constant::src_color)
        {
            return MTLBlendFactorSourceColor;
        }
        
        if (value == gl::constant::one_minus_src_alpha)
        {
            return MTLBlendFactorOneMinusSourceAlpha;
        }
        
        assert(false);
        return MTLBlendFactorZero;
    }
    
    MTLBlendOperation mtl_render_pipeline_state_impl::convert_blend_operation_from_gl_to_mtl(ui32 value)
    {
        if (value == gl::constant::func_add)
        {
            return MTLBlendOperationAdd;
        }
        
        if (value == gl::constant::func_min)
        {
            return MTLBlendOperationMin;
        }
        
        if (value == gl::constant::func_max)
        {
            return MTLBlendOperationMax;
        }
        
        assert(false);
        return MTLBlendOperationAdd;
    }
    
    void* mtl_render_pipeline_state_impl::get_mtl_raw_render_pipeline_state_ptr() const
    {
        return (__bridge void*)m_render_pipeline_state;
    }
    
    mtl_render_pipeline_state::mtl_render_pipeline_state(const std::shared_ptr<material_cached_parameters>& material_parameters,
                                                         const mtl_vertex_descriptor_shared_ptr& vertex_descriptor)
    {
        m_mtl_render_pipeline_state_impl = std::make_shared<mtl_render_pipeline_state_impl>(material_parameters,
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
