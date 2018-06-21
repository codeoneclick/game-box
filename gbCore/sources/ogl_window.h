//
//  ogl_window.h
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "main_headers.h"

#if defined(__IOS__) || defined(__TVOS__)

#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>

@interface opengl_view : UIView

@end

#endif

namespace gb
{
    class ogl_window
    {
    private:
        
#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)

		void* m_hwnd;

#elif defined(__WINOS__)

		HWND m_hwnd;
		HDC	m_hdc;
		glm::ivec2 m_size;

#endif
        
    protected:
        
    public:
        
        ogl_window(void* hwnd);
        ~ogl_window();
        
        ui32 get_width() const;
        ui32 get_height() const;
        
#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)

        const void* get_hwnd() const;

#elif defined(__WINOS__)

		HWND get_hwnd() const;
		HDC get_hdc() const;

#endif
    };
};

#endif
