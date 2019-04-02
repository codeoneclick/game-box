//
//  mtl_render_pass_descriptor.hpp
//  gbCore
//
//  Created by serhii.s on 3/18/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//
#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API == METAL_API

#include "declarations.h"
#include "ws_technique_configuration.h"
#include "ss_technique_configuration.h"

namespace gb
{
    class i_mtl_render_pass_descriptor_impl : public std::enable_shared_from_this<i_mtl_render_pass_descriptor_impl>
    {
    public:
        
        template <typename T>
        std::shared_ptr<T> as() const
        {
            return std::static_pointer_cast<T>(shared_from_this());
        };
        
        virtual void* get_mtl_render_pass_descriptor_ptr() const = 0;
        virtual void* get_mtl_render_commnad_encoder() const = 0;
        virtual void* get_mtl_render_encoder(const std::string& guid) const = 0;
        
        virtual ui32 get_color_attachments_num() const = 0;
        virtual bool is_color_attachment_exist(i32 index) const = 0;
        virtual ui64 get_color_attachment_pixel_format(i32 index) const = 0;
        virtual ui64 get_depth_stencil_attachment_pixel_format() const = 0;
        
        virtual std::vector<texture_shared_ptr> get_color_attachments_texture() = 0;
        
        virtual void bind() = 0;
        virtual void unbind() = 0;
    };
    
    class mtl_render_pass_descriptor : public std::enable_shared_from_this<mtl_render_pass_descriptor>
    {
    private:
        
        std::string m_name;
        std::shared_ptr<i_mtl_render_pass_descriptor_impl> m_render_pass_descriptor_impl = nullptr;
        
    protected:
        
        template <typename T>
        std::shared_ptr<T> impl_as() const
        {
            return std::static_pointer_cast<T>(m_render_pass_descriptor_impl);
        };
        
    public:
        
        mtl_render_pass_descriptor(const std::shared_ptr<ws_technique_configuration>& configuration);
        mtl_render_pass_descriptor(const std::shared_ptr<ss_technique_configuration>& configuration);
        mtl_render_pass_descriptor(const std::string& name, void* mtl_raw_color_attachment_ptr, void* mtl_raw_depth_stencil_attachment_ptr);
        ~mtl_render_pass_descriptor();
 
        void* get_mtl_render_pass_descriptor_ptr() const;
        void* get_mtl_render_commnad_encoder() const;
        void* get_mtl_render_encoder(const std::string& guid) const;
        
        ui32 get_color_attachments_num() const;
        bool is_color_attachment_exist(i32 index) const;
        ui64 get_color_attachment_pixel_format(i32 index) const;
        ui64 get_depth_stencil_attachment_pixel_format() const;
        
        std::vector<texture_shared_ptr> get_color_attachments_texture();
        
        void bind();
        void unbind();
    };
};

#endif

