//
//  ogl_window_tvos.cpp
//  gbCore
//
//  Created by serhii serhiiv on 4/5/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//
#if defined(__TVOS__) && !defined(__NO_RENDER__)

#include "ogl_window.h"
#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>

@implementation opengl_view

+ (Class)layerClass
{
    return [CAEAGLLayer class];
}

- (instancetype)initWithFrame:(CGRect)frame;
{
    if (self = [super initWithFrame:frame])
    {
        super.layer.opaque = YES;
    }
    return self;
}

@end

namespace gb
{
    ogl_window::ogl_window(void* hwnd) :
    m_hwnd(hwnd)
    {
        assert(m_hwnd != nullptr);
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
        const UIView* hwnd = (__bridge UIView*)m_hwnd;
        return static_cast<ui32>(hwnd.frame.size.width);
    }
    
    ui32 ogl_window::get_height() const
    {
        assert(m_hwnd != nullptr);
        const UIView* hwnd = (__bridge UIView*)m_hwnd;
        return static_cast<ui32>(hwnd.frame.size.height);
    }
}

#endif

