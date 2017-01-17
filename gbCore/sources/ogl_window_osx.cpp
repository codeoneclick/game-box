//
//  ogl_window.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//
#if defined(__OSX__) && !defined(__NO_RENDER__)

#include "ogl_window.h"
#include <Cocoa/Cocoa.h>

namespace gb
{
    ogl_window::ogl_window(void* hwnd) :
    m_hwnd(hwnd)
    {
        assert(m_hwnd);
    }
    
    ogl_window::~ogl_window()
    {
        
    }
    
    const void* ogl_window::get_hwnd() const
    {
        return m_hwnd;
    }
    
    ui32 ogl_window::get_width() const
    {
        assert(m_hwnd != nullptr);
        const NSOpenGLView *hwnd = (__bridge NSOpenGLView *)m_hwnd;
        return static_cast<ui32>(hwnd.frame.size.width);
    }
    
    ui32 ogl_window::get_height() const
    {
        assert(m_hwnd != nullptr);
        const NSOpenGLView* hwnd = (__bridge NSOpenGLView *)m_hwnd;
        return static_cast<ui32>(hwnd.frame.size.height);
    }
}

#endif
