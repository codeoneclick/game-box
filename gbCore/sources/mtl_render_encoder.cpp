//
//  mtl_render_encoder.cpp
//  gbCore
//
//  Created by serhii.s on 3/11/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#include "mtl_render_encoder.h"
#include "mtl_device.h"
#include "mtl_render_pipeline_state.h"
#include "mtl_depth_stencil_state.h"
#include "mtl_texture.h"
#include "mtl_buffer.h"

#if USED_GRAPHICS_API == METAL_API

#include <MetalKit/MetalKit.h>

namespace gb
{
    class mtl_render_encoder_impl : public i_mtl_render_encoder_impl
    {
    private:
        
        id<MTLRenderCommandEncoder> m_render_encoder = nil;
        id<MTLBuffer> m_index_buffer = nil;
        ui32 m_indices_count = 0;
        ui32 m_indices_offset = 0;
        
    protected:
        
    public:
        
        mtl_render_encoder_impl();
        ~mtl_render_encoder_impl();
        
        void bind() override;
        void unbind() override;
        
        void set_render_pipeline_state(const mtl_render_pipeline_state_shared_ptr& render_pipeline_state) override;
        void set_depth_stencil_state(const mtl_depth_stencil_state_shared_ptr& depth_stencil_state) override;
        void set_texture(const mtl_texture_shared_ptr& texture, ui32 index) override;
        void set_vertex_buffer(const mtl_buffer_shared_ptr& buffer, ui32 index) override;
        void set_index_buffer(const mtl_buffer_shared_ptr& buffer, ui32 indices_count, ui32 indices_offset) override;
        
        void draw() override;
    };
    
    mtl_render_encoder_impl::mtl_render_encoder_impl()
    {
        
    }
    
    mtl_render_encoder_impl::~mtl_render_encoder_impl()
    {
        
    }
    
    void mtl_render_encoder_impl::bind()
    {
        m_render_encoder = (__bridge id<MTLRenderCommandEncoder>)gb::mtl_device::get_instance()->get_mtl_render_encoder("main.loop");
    }
    
    void mtl_render_encoder_impl::unbind()
    {
        
    }
    
    void mtl_render_encoder_impl::set_render_pipeline_state(const mtl_render_pipeline_state_shared_ptr& render_pipeline_state)
    {
        id<MTLRenderPipelineState> mtl_raw_render_pipeline_state = (__bridge id<MTLRenderPipelineState>)render_pipeline_state->get_mtl_raw_render_pipeline_state_ptr();
        [m_render_encoder setRenderPipelineState:mtl_raw_render_pipeline_state];
    }
    
    void mtl_render_encoder_impl::set_depth_stencil_state(const mtl_depth_stencil_state_shared_ptr& depth_stencil_state)
    {
        id<MTLDepthStencilState> mtl_raw_depth_stencil_state = (__bridge id<MTLDepthStencilState>)depth_stencil_state->get_mtl_raw_depth_stencil_state_ptr();
        [m_render_encoder setDepthStencilState:mtl_raw_depth_stencil_state];
    }
    
    void mtl_render_encoder_impl::set_texture(const mtl_texture_shared_ptr& texture, ui32 index)
    {
        id<MTLTexture> mtl_raw_texture = (__bridge id<MTLTexture>)texture->get_mtl_raw_texture_ptr();
        [m_render_encoder setFragmentTexture:mtl_raw_texture atIndex:index];
    }
    
    void mtl_render_encoder_impl::set_vertex_buffer(const mtl_buffer_shared_ptr& buffer, ui32 index)
    {
        id<MTLBuffer> mtl_raw_buffer = (__bridge id<MTLBuffer>)buffer->get_mtl_raw_buffer_ptr();
        [m_render_encoder setVertexBuffer:mtl_raw_buffer offset:0 atIndex:index];
    }
    
    void mtl_render_encoder_impl::set_index_buffer(const mtl_buffer_shared_ptr& buffer, ui32 indices_count, ui32 indices_offset)
    {
        m_index_buffer = (__bridge id<MTLBuffer>)buffer->get_mtl_raw_buffer_ptr();
        m_indices_count = indices_count;
        m_indices_offset = indices_offset;
    }
    
    void mtl_render_encoder_impl::draw()
    {
        [m_render_encoder drawIndexedPrimitives:MTLPrimitiveTypeTriangle
                                  indexCount:m_indices_count
                                   indexType:MTLIndexTypeUInt16
                                 indexBuffer:m_index_buffer
                           indexBufferOffset:0];
    }
    
    mtl_render_encoder::mtl_render_encoder()
    {
        m_render_encoder_impl = std::make_shared<mtl_render_encoder_impl>();
    }
    
    mtl_render_encoder::~mtl_render_encoder()
    {
        
    }
    
    void mtl_render_encoder::bind()
    {
        impl_as<mtl_render_encoder_impl>()->bind();
    }
    
    void mtl_render_encoder::unbind()
    {
        impl_as<mtl_render_encoder_impl>()->unbind();
    }
    
    void mtl_render_encoder::set_render_pipeline_state(const mtl_render_pipeline_state_shared_ptr& render_pipeline_state)
    {
        impl_as<mtl_render_encoder_impl>()->set_render_pipeline_state(render_pipeline_state);
    }
    
    void mtl_render_encoder::set_depth_stencil_state(const mtl_depth_stencil_state_shared_ptr& depth_stencil_state)
    {
        impl_as<mtl_render_encoder_impl>()->set_depth_stencil_state(depth_stencil_state);
    }
    
    void mtl_render_encoder::set_texture(const mtl_texture_shared_ptr& texture, ui32 index)
    {
        impl_as<mtl_render_encoder_impl>()->set_texture(texture, index);
    }
    
    void mtl_render_encoder::set_vertex_buffer(const mtl_buffer_shared_ptr& buffer, ui32 index)
    {
        impl_as<mtl_render_encoder_impl>()->set_vertex_buffer(buffer, index);
    }
    
    void mtl_render_encoder::set_index_buffer(const mtl_buffer_shared_ptr& buffer, ui32 indices_count, ui32 indices_offset)
    {
        impl_as<mtl_render_encoder_impl>()->set_index_buffer(buffer, indices_count, indices_offset);
    }
    
    void mtl_render_encoder::draw()
    {
        impl_as<mtl_render_encoder_impl>()->draw();
    }
}

#endif
