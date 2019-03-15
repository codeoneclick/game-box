//
//  mtl_buffer.h
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
    class i_mtl_buffer_impl : public std::enable_shared_from_this<i_mtl_buffer_impl>
    {
    public:
        
        template <typename T>
        std::shared_ptr<T> as() const
        {
            return std::static_pointer_cast<T>(shared_from_this());
        };
        
        virtual void* get_mtl_raw_buffer_ptr() const = 0;
    };
    
    class mtl_buffer
    {
    private:
        
        std::shared_ptr<i_mtl_buffer_impl> m_buffer_impl = nullptr;
        
    protected:
        
        template <typename T>
        std::shared_ptr<T> impl_as() const
        {
            return std::static_pointer_cast<T>(m_buffer_impl);
        };
        
    public:
        
        mtl_buffer(ui32 size);
        mtl_buffer(void* data, ui32 size);
        ~mtl_buffer();
        
        void update(void* data, ui32 size);
        
        void* get_mtl_raw_buffer_ptr() const;
    };
};

#endif

