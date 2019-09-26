//
//  window_impl.h
//  gbCore
//
//  Created by sergey.sergeev on 8/12/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

namespace gb
{
    class window_impl
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
        
        window_impl(void* hwnd);
        ~window_impl();
        
        glm::ivec2 get_resolution_size_in_pixels();
        glm::ivec2 get_resolution_size_in_points();
        
        
#if defined(__IOS__) || defined(__OSX__) || defined(__TVOS__)

        const void* get_hwnd() const;

#elif defined(__WINOS__)

		HWND get_hwnd() const;
		HDC get_hdc() const;

#endif
        
    };
};

#endif
