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
    };
    
    class mtl_device
    {
    private:
        
        static std::shared_ptr<mtl_device> m_instance;
        
        std::shared_ptr<i_mtl_device_impl> m_device_impl = nullptr;
        
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
        
        void setup(const void* hwnd);
    };
};

#endif

