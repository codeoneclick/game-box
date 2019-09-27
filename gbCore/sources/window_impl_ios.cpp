//
//  ogl_window_ios.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "window_impl.h"

#if defined(__IOS__) && USED_GRAPHICS_API != NO_GRAPHICS_API

#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>

#if USED_GRAPHICS_API == METAL_API

#import <MetalKit/MetalKit.h>

#endif

namespace gb
{
    window_impl::window_impl(void* hwnd) :
    m_hwnd(hwnd)
    {
        assert(m_hwnd != nullptr);
        const MTKView* _hwnd = (__bridge MTKView*)m_hwnd;
        std::cout<<"[resolution in pixels] : "<<_hwnd.drawableSize.width<<", "<<_hwnd.drawableSize.height<<" scale: "<<_hwnd.contentScaleFactor<<std::endl;
    }
    
    window_impl::~window_impl()
    {
        
    }
    
    const void* window_impl::get_hwnd() const
    {
        return m_hwnd;
    }
    
    glm::ivec2 window_impl::get_resolution_size_in_pixels()
    {
        assert(m_hwnd != nullptr);
        const MTKView* hwnd = (__bridge MTKView*)m_hwnd;
        auto screen_resolution = glm::ivec2(hwnd.drawableSize.width, hwnd.drawableSize.height);
        if (screen_resolution.x > 2000.f)
        {
            screen_resolution.x *= 0.5f;
            screen_resolution.y *= 0.5f;
        }
        return screen_resolution;
    }

    glm::ivec2 window_impl::get_resolution_size_in_points()
    {
        const auto screen_bounds = glm::ivec2([[UIScreen mainScreen] bounds].size.width,
                                              [[UIScreen mainScreen] bounds].size.height);
        return glm::ivec2(960, 960 * (static_cast<f32>(screen_bounds.y) / static_cast<f32>(screen_bounds.x)));
    }
}

#endif
