//
//  ogl_window_tvos.cpp
//  gbCore
//
//  Created by serhii serhiiv on 4/5/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "window_impl.h"

#if defined(__TVOS__) && USED_GRAPHICS_API != NO_GRAPHICS_API

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
    window_impl::window_impl(void* hwnd) :
    m_hwnd(hwnd)
    {
        assert(m_hwnd != nullptr);
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
        const UIView* hwnd = (__bridge UIView*)m_hwnd;
        return static_cast<ui32>(hwnd.frame.size.width);
    }
    
    ui32 window_impl::get_height() const
    {
        assert(m_hwnd != nullptr);
        const UIView* hwnd = (__bridge UIView*)m_hwnd;
        return static_cast<ui32>(hwnd.frame.size.height);
    }
}

#endif

