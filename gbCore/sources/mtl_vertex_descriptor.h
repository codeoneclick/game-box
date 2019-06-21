//
//  mtl_vertex_descriptor.hpp
//  gbCore
//
//  Created by serhii.s on 3/15/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//
#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API == METAL_API

#include "declarations.h"

namespace gb
{
    class i_mtl_vertex_descriptor_impl : public std::enable_shared_from_this<i_mtl_vertex_descriptor_impl>
    {
    public:
        
        template <typename T>
        std::shared_ptr<T> as() const
        {
            return std::static_pointer_cast<T>(shared_from_this());
        };
        
        virtual void* get_mtl_vertex_descriptor_ptr() const = 0;
    };
    
    class mtl_vertex_descriptor
    {
    private:
        
        std::shared_ptr<i_mtl_vertex_descriptor_impl> m_vertex_descriptor_impl = nullptr;
        
    protected:
        
        template <typename T>
        std::shared_ptr<T> impl_as() const
        {
            return std::static_pointer_cast<T>(m_vertex_descriptor_impl);
        };
        
    public:
        
        mtl_vertex_descriptor();
        ~mtl_vertex_descriptor();
        
        static std::shared_ptr<mtl_vertex_descriptor> create_PTNTC_vertex_descriptor();
        static std::shared_ptr<mtl_vertex_descriptor> create_PTC_vertex_descriptor();
        
        void* get_mtl_vertex_descriptor_ptr() const;
    };
};

#endif
