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
        static ui32 m_cached_render_encoder_revision;
        static std::string m_cached_render_technique_name;
        bool m_is_same_revision = false;
        id<MTLBuffer> m_index_buffer = nil;
        ui32 m_indices_count = 0;
        ui32 m_indices_offset = 0;
        
        static std::string m_cached_render_pipeline_state_guid;
        static std::string m_cached_depth_stencil_pipeline_state_guid;
        
    protected:
        
    public:
        
        mtl_render_encoder_impl();
        ~mtl_render_encoder_impl();
        
        void bind(const std::string& technique_name) override;
        void unbind(const std::string& technique_name) override;
        
        void set_render_pipeline_state(const mtl_render_pipeline_state_shared_ptr& render_pipeline_state) override;
        void set_depth_stencil_state(const mtl_depth_stencil_state_shared_ptr& depth_stencil_state) override;
        void set_texture(const mtl_texture_shared_ptr& texture, ui32 index) override;
        void set_vertex_buffer(const mtl_buffer_shared_ptr& buffer, ui32 index) override;
        void set_index_buffer(const mtl_buffer_shared_ptr& buffer, ui32 indices_count, ui32 indices_offset) override;
        void set_vertex_uniforms(const mtl_buffer_shared_ptr& buffer, ui32 index) override;
        void set_fragment_uniforms(const mtl_buffer_shared_ptr& buffer, ui32 index) override;
        void set_cull_mode(ui32 cull_mode) override;
        void set_cull_mode_none() override;
        void set_stencil_value(ui8 value) override;
        
        void draw(const std::string& technique_name) override;
    };
    
    ui32 mtl_render_encoder_impl::m_cached_render_encoder_revision = std::numeric_limits<ui32>::max();
    std::string mtl_render_encoder_impl::m_cached_render_technique_name = "undedfined";
    std::string mtl_render_encoder_impl::m_cached_render_pipeline_state_guid = "undefined";
    std::string mtl_render_encoder_impl::m_cached_depth_stencil_pipeline_state_guid = "undefined";
    
    mtl_render_encoder_impl::mtl_render_encoder_impl()
    {
        
    }
    
    mtl_render_encoder_impl::~mtl_render_encoder_impl()
    {
        
    }
    
    void mtl_render_encoder_impl::bind(const std::string& technique_name)
    {
        ui32 current_render_encoder_revision = 0;
        m_render_encoder = (__bridge id<MTLRenderCommandEncoder>)gb::mtl_device::get_instance()->get_mtl_render_encoder(&current_render_encoder_revision);
        if (current_render_encoder_revision != m_cached_render_encoder_revision || m_cached_render_technique_name != technique_name)
        {
            m_is_same_revision = false;
            m_cached_render_encoder_revision = current_render_encoder_revision;
            m_cached_render_technique_name = technique_name;
            
            m_cached_render_pipeline_state_guid = "undefined";
            m_cached_depth_stencil_pipeline_state_guid = "undefined";
        }
    }
    
    void mtl_render_encoder_impl::unbind(const std::string& technique_name)
    {
        
    }
    
    void mtl_render_encoder_impl::set_render_pipeline_state(const mtl_render_pipeline_state_shared_ptr& render_pipeline_state)
    {
        id<MTLRenderPipelineState> mtl_raw_render_pipeline_state = (__bridge id<MTLRenderPipelineState>)render_pipeline_state->get_mtl_raw_render_pipeline_state_ptr();
        const auto render_pipeline_state_guid = render_pipeline_state->get_guid();
        if (render_pipeline_state_guid != m_cached_render_pipeline_state_guid)
        {
            [m_render_encoder setRenderPipelineState:mtl_raw_render_pipeline_state];
            m_cached_render_pipeline_state_guid = render_pipeline_state_guid;
        }
    }
    
    void mtl_render_encoder_impl::set_depth_stencil_state(const mtl_depth_stencil_state_shared_ptr& depth_stencil_state)
    {
        id<MTLDepthStencilState> mtl_raw_depth_stencil_state = (__bridge id<MTLDepthStencilState>)depth_stencil_state->get_mtl_raw_depth_stencil_state_ptr();
        const auto depth_stencil_state_guid = depth_stencil_state->get_guid();
        if (depth_stencil_state_guid != m_cached_depth_stencil_pipeline_state_guid)
        {
            [m_render_encoder setDepthStencilState:mtl_raw_depth_stencil_state];
            m_cached_depth_stencil_pipeline_state_guid = depth_stencil_state_guid;
        }
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
    
    void mtl_render_encoder_impl::set_vertex_uniforms(const mtl_buffer_shared_ptr& buffer, ui32 index)
    {
        id<MTLBuffer> mtl_raw_buffer = (__bridge id<MTLBuffer>)buffer->get_mtl_raw_buffer_ptr();
        [m_render_encoder setVertexBuffer:mtl_raw_buffer offset:0 atIndex:index];
    }
    
    void mtl_render_encoder_impl::set_fragment_uniforms(const mtl_buffer_shared_ptr& buffer, ui32 index)
    {
        id<MTLBuffer> mtl_raw_buffer = (__bridge id<MTLBuffer>)buffer->get_mtl_raw_buffer_ptr();
        [m_render_encoder setFragmentBuffer:mtl_raw_buffer offset:0 atIndex:index];
    }
    
    void mtl_render_encoder_impl::set_cull_mode(ui32 cull_mode)
    {
        [m_render_encoder setCullMode:cull_mode == gl::constant::back ? MTLCullModeBack : MTLCullModeFront];
    }
    
    void mtl_render_encoder_impl::set_cull_mode_none()
    {
        [m_render_encoder setCullMode:MTLCullModeNone];
    }
    
    void mtl_render_encoder_impl::set_stencil_value(ui8 value)
    {
        [m_render_encoder setStencilReferenceValue:value];
    }
    
    void mtl_render_encoder_impl::draw(const std::string& technique_name)
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
    
    void mtl_render_encoder::bind(const std::string& technique_name)
    {
        impl_as<mtl_render_encoder_impl>()->bind(technique_name);
    }
    
    void mtl_render_encoder::unbind(const std::string& technique_name)
    {
        impl_as<mtl_render_encoder_impl>()->unbind(technique_name);
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
    
    void mtl_render_encoder::set_vertex_uniforms(const mtl_buffer_shared_ptr& buffer, ui32 index)
    {
        impl_as<mtl_render_encoder_impl>()->set_vertex_uniforms(buffer, index);
    }
    
    void mtl_render_encoder::set_fragment_uniforms(const mtl_buffer_shared_ptr& buffer, ui32 index)
    {
        impl_as<mtl_render_encoder_impl>()->set_fragment_uniforms(buffer, index);
    }
    
    void mtl_render_encoder::set_cull_mode(ui32 cull_mode)
    {
        impl_as<mtl_render_encoder_impl>()->set_cull_mode(cull_mode);
    }
    
    void mtl_render_encoder::set_cull_mode_none()
    {
        impl_as<mtl_render_encoder_impl>()->set_cull_mode_none();
    }
    
    void mtl_render_encoder::set_stencil_value(ui8 value)
    {
        impl_as<mtl_render_encoder_impl>()->set_stencil_value(value);
    }
    
    void mtl_render_encoder::draw(const std::string& technique_name)
    {
        impl_as<mtl_render_encoder_impl>()->draw(technique_name);
    }
}

#endif
