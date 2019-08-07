//
//  graphics_context.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "declarations.h"

namespace gb
{
    enum e_graphic_context_api
    {
        e_graphic_context_api_osx = 1,
        e_graphic_context_api_ios,
		e_graphic_context_api_win32,
        e_graphic_context_api_tvos
    };
    
    class window_impl;
    class graphics_context : public std::enable_shared_from_this<graphics_context>
    {
    private:
        
    protected:
        
        ui32 m_frame_buffer;
        ui32 m_render_buffer;
        ui32 m_depth_buffer;
        ui32 m_msaa_frame_buffer;
        ui32 m_msaa_render_buffer;
        
        std::shared_ptr<window_impl> m_window;
        
        static graphics_context_shared_ptr m_current_context;
        
        graphics_context() = default;
        
    public:
       
        static graphics_context_shared_ptr construct(const std::shared_ptr<window_impl>& window,
                                                           e_graphic_context_api api);
        
        static graphics_context_shared_ptr get_current_context();
        
#if defined(__IOS__)
      
        static bool is_ipad();
        
#endif
        
        virtual ~graphics_context() = default;
        
        virtual ui32 get_frame_buffer() const;
        virtual ui32 get_render_buffer() const;
        ui32 get_depth_buffer() const;
        
        ui32 get_width() const;
        ui32 get_height() const;
        
        virtual void* get_context() const = 0;
        
        virtual void make_current() = 0;
        virtual void draw() const = 0;
    };
};

#endif
