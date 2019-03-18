//
//  mtl_device.hpp
//  gbCore
//
//  Created by serhii.s on 3/1/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API == METAL_API

#include "declarations.h"

namespace gb
{
    class i_mtl_device_impl : public std::enable_shared_from_this<i_mtl_device_impl>
    {
    public:
        
        template <typename T>
        std::shared_ptr<T> as() const
        {
            return std::static_pointer_cast<T>(shared_from_this());
        };
        
        virtual void* get_mtl_raw_device_ptr() const = 0;
        virtual void* get_mtl_raw_library_ptr() const = 0;
        virtual void* get_mtl_raw_command_queue_ptr() const = 0;
        virtual void* get_mtl_raw_command_buffer_ptr() const = 0;
        virtual void* get_mtl_raw_color_attachment_ptr() const = 0;
        virtual void* get_mtl_raw_depth_stencil_attachment_ptr() const = 0;
        
        virtual ui64 get_samples_count() const = 0;
        
        virtual ui64 get_color_pixel_format() const = 0;
        virtual ui64 get_depth_stencil_pixel_format() const = 0;
        
        virtual void bind() = 0;
        virtual void unbind() = 0;
    };
    
    class mtl_device
    {
    private:
        
        static std::shared_ptr<mtl_device> m_instance;
        
        std::shared_ptr<i_mtl_device_impl> m_device_impl = nullptr;
        mtl_render_pass_descriptor_shared_ptr m_current_render_pass_descriptor = nullptr;
        
    protected:
        
        static std::shared_ptr<mtl_device> construct();
        
        template <typename T>
        std::shared_ptr<T> impl_as() const
        {
            return std::static_pointer_cast<T>(m_device_impl);
        };
        
    public:
        
        ~mtl_device();
        static std::shared_ptr<mtl_device> get_instance();
        
        void init(const void* hwnd);
        
        void bind();
        void unbind();
        
        void* get_mtl_raw_device_ptr() const;
        void* get_mtl_raw_library_ptr() const;
        void* get_mtl_raw_command_queue_ptr() const;
        void* get_mtl_raw_command_buffer_ptr() const;
        void* get_mtl_render_encoder(const std::string& guid) const;
        void* get_mtl_raw_color_attachment_ptr() const;
        void* get_mtl_raw_depth_stencil_attachment_ptr() const;
        
        ui64 get_samples_count() const;
        
        ui64 get_color_pixel_format() const;
        ui64 get_depth_stencil_pixel_format() const;
        
        void set_current_render_pass_descriptor(const mtl_render_pass_descriptor_shared_ptr& render_pass_descriptor);
        mtl_render_pass_descriptor_shared_ptr get_current_render_pass_descriptor() const;
    };
};

#endif

