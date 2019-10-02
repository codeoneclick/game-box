//
//  mtl_texture.h
//  gbCore
//
//  Created by serhii.s on 3/6/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//
#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API == METAL_API

#include "declarations.h"

namespace gb
{
    class i_mtl_texture_impl : public std::enable_shared_from_this<i_mtl_texture_impl>
    {
    public:
        
        template <typename T>
        std::shared_ptr<T> as() const
        {
            return std::static_pointer_cast<T>(shared_from_this());
        };
        
        virtual void* get_mtl_raw_texture_ptr() const = 0;
        virtual void update_pixels_data(ui32 width, ui32 height, void* pixels, ui32 format = gl::constant::rgba_t) = 0;
    };
    
    class mtl_texture
    {
    private:
        
        std::shared_ptr<i_mtl_texture_impl> m_texture_impl = nullptr;
        bool m_is_mipmaps_generated = false;
        
    protected:
        
        template <typename T>
        std::shared_ptr<T> impl_as() const
        {
            return std::static_pointer_cast<T>(m_texture_impl);
        };
        
    public:
        
        mtl_texture(ui32 width, ui32 height, void* pixels, ui32 format = gl::constant::rgba_t);
        mtl_texture(ui32 size, std::array<ui8*, 6> pixels, ui32 format = gl::constant::rgba_t);
        mtl_texture(void* mtl_texture_descriptor);
        ~mtl_texture();
        
        void* get_mtl_raw_texture_ptr() const;
        
        void set_mipmaps_generated(bool value);
        bool get_is_mipmaps_generated() const;
        
        void update_pixels_data(ui32 width, ui32 height, void* pixels, ui32 format = gl::constant::rgba_t);
    };
};

#endif
