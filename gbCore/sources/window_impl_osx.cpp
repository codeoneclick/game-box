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
    
    ui32 window_impl::get_width() const
    {
        assert(m_hwnd != nullptr);
        const NSOpenGLView *hwnd = (__bridge NSOpenGLView *)m_hwnd;
        return static_cast<ui32>(hwnd.frame.size.width);
    }
    
    ui32 window_impl::get_height() const
    {
        assert(m_hwnd != nullptr);
        const NSOpenGLView* hwnd = (__bridge NSOpenGLView *)m_hwnd;
        return static_cast<ui32>(hwnd.frame.size.height);
    }
}

#endif
