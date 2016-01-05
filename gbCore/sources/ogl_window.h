//
//  ogl_window.h
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ogl_window_h
#define ogl_window_h

#include "main_headers.h"

#if defined(__IOS__)

#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>

@interface opengl_view : UIView

@property (nonatomic, assign) UIInterfaceOrientation orientation;

@end

#endif

namespace gb
{
    class ogl_window
    {
    private:
        
        void* m_hwnd;
        
    protected:
        
    public:
        
        ogl_window(void* hwnd);
        ~ogl_window(void);
        
        ui32 get_width(void) const;
        ui32 get_height(void) const;
        
        const void* get_hwnd(void) const;
    };
};

#endif
