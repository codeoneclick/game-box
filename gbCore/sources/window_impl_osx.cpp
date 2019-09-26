//
//  ogl_window.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "window_impl.h"

#if defined(__OSX__) && USED_GRAPHICS_API != NO_GRAPHICS_API

#include <Cocoa/Cocoa.h>

#if USED_GRAPHICS_API == METAL_API

#import <MetalKit/MetalKit.h>

#endif

namespace gb
{
    window_impl::window_impl(void* hwnd) :
    m_hwnd(hwnd)
    {
        assert(m_hwnd);
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
        NSRect frame = [[NSScreen mainScreen] visibleFrame];
        
    #if defined(DEBUG)
        
        frame.origin.x = frame.size.width * 0.25 * 0.5;
        frame.origin.y = frame.size.height * 0.25;
        frame.size.width *= 0.75;
        frame.size.height *= 0.75;
        
    #endif
        
        return glm::ivec2(frame.size.width,
                          frame.size.height);
    }

    glm::ivec2 window_impl::get_resolution_size_in_points()
    {
            NSRect frame = [[NSScreen mainScreen] visibleFrame];
            
        #if defined(DEBUG)
            
            frame.origin.x = frame.size.width * 0.25 * 0.5;
            frame.origin.y = frame.size.height * 0.25;
            frame.size.width *= 0.75;
            frame.size.height *= 0.75;
            
        #endif
        
        return glm::ivec2(frame.size.width,
                          frame.size.height);
    }
}

#endif
