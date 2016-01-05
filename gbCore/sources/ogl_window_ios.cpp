//
//  ogl_window_ios.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "ogl_window.h"
#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>

#if defined(__IOS__)

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

#endif

namespace gb
{
    ogl_window::ogl_window(void* hwnd) :
    m_hwnd(hwnd)
    {
        assert(m_hwnd != nullptr);
    }
    
    ogl_window::~ogl_window(void)
    {
        
    }
    
    const void* ogl_window::get_hwnd(void) const
    {
        return m_hwnd;
    }
    
    ui32 ogl_window::get_width(void) const
    {
        assert(m_hwnd != nullptr);
        const UIView* hwnd = (__bridge UIView*)m_hwnd;
        return static_cast<ui32>(std::max(hwnd.frame.size.width, hwnd.frame.size.height));
    }
    
    ui32 ogl_window::get_height(void) const
    {
        assert(m_hwnd != nullptr);
        const UIView* hwnd = (__bridge UIView*)m_hwnd;
        return static_cast<ui32>(std::min(hwnd.frame.size.width, hwnd.frame.size.height));
    }
}