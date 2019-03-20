//
//  mtl_render_encoder.hpp
//  gbCore
//
//  Created by serhii.s on 3/11/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//
#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API == METAL_API

#include "declarations.h"

namespace gb
{
    class i_mtl_render_encoder_impl : public std::enable_shared_from_this<i_mtl_render_encoder_impl>
    {
    public:
        
        template <typename T>
        std::shared_ptr<T> as() const
        {
            return std::static_pointer_cast<T>(shared_from_this());
        };
        
        virtual void bind(const std::string& technique_name) = 0;
        virtual void unbind(const std::string& technique_name) = 0;
        
        virtual void set_render_pipeline_state(const mtl_render_pipeline_state_shared_ptr& render_pipeline_state) = 0;
        virtual void set_depth_stencil_state(const mtl_depth_stencil_state_shared_ptr& depth_stencil_state) = 0;
        virtual void set_texture(const mtl_texture_shared_ptr& texture, ui32 index) = 0;
        virtual void set_vertex_buffer(const mtl_buffer_shared_ptr& buffer, ui32 index) = 0;
        virtual void set_index_buffer(const mtl_buffer_shared_ptr& buffer, ui32 indices_count, ui32 indices_offset) = 0;
        virtual void set_vertex_uniforms(const mtl_buffer_shared_ptr& buffer, ui32 index) = 0;
        virtual void set_fragment_uniforms(const mtl_buffer_shared_ptr& buffer, ui32 index) = 0;
        virtual void set_cull_mode(ui32 cull_mode) = 0;
        virtual void set_cull_mode_none() = 0;
        
        virtual void draw(const std::string& technique_name) = 0;
    };
    
    class mtl_render_encoder
    {
    private:
        
        std::shared_ptr<i_mtl_render_encoder_impl> m_render_encoder_impl = nullptr;
        
    protected:
        
        template <typename T>
        std::shared_ptr<T> impl_as() const
        {
            return std::static_pointer_cast<T>(m_render_encoder_impl);
        };
        
    public:
        
        mtl_render_encoder();
        ~mtl_render_encoder();
        
        void bind(const std::string& technique_name);
        void unbind(const std::string& technique_name);
        
        void set_render_pipeline_state(const mtl_render_pipeline_state_shared_ptr& render_pipeline_state);
        void set_depth_stencil_state(const mtl_depth_stencil_state_shared_ptr& depth_stencil_state);
        void set_texture(const mtl_texture_shared_ptr& texture, ui32 index);
        void set_vertex_buffer(const mtl_buffer_shared_ptr& buffer, ui32 index);
        void set_index_buffer(const mtl_buffer_shared_ptr& buffer, ui32 indices_count, ui32 indices_offset);
        void set_vertex_uniforms(const mtl_buffer_shared_ptr& buffer, ui32 index);
        void set_fragment_uniforms(const mtl_buffer_shared_ptr& buffer, ui32 index);
        void set_cull_mode(ui32 cull_mode);
        void set_cull_mode_none();
        
        void draw(const std::string& technique_name);
    };
};

#endif
