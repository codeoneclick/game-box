//
//  mtl_depth_stencil_state.h
//  gbCore
//
//  Created by serhii.s on 3/5/19.
//  Copyright © 2019 sergey.sergeev. All rights reserved.
//
#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API == METAL_API

#include "material.h"

namespace gb
{
    class i_mtl_depth_stencil_state_impl : public std::enable_shared_from_this<i_mtl_depth_stencil_state_impl>
    {
    public:
        
        template <typename T>
        std::shared_ptr<T> as() const
        {
            return std::static_pointer_cast<T>(shared_from_this());
        };
        
        virtual void* get_mtl_raw_depth_stencil_state_ptr() const = 0;
    };
    
    class mtl_depth_stencil_state
    {
    private:
        
        std::shared_ptr<i_mtl_depth_stencil_state_impl> m_mtl_depth_stencil_state_impl = nullptr;
        
    protected:
        
        template <typename T>
        std::shared_ptr<T> impl_as() const
        {
            return std::static_pointer_cast<T>(m_mtl_depth_stencil_state_impl);
        };
        
    public:
        
        mtl_depth_stencil_state(const std::shared_ptr<material_cached_parameters>& material_parameters);
        ~mtl_depth_stencil_state();
        
        void* get_mtl_raw_depth_stencil_state_ptr() const;
    };
};

#endif


