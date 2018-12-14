//
//  graphics_context_osx.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "graphics_context.h"

#if defined(__OSX__) && USED_GRAPHICS_API != NO_GRAPHICS_API

#include "ogl_window.h"
#include <Cocoa/Cocoa.h>

namespace gb
{
    static NSOpenGLPixelFormatAttribute g_attributes[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 24,
#if USED_GRAPHICS_API == OPENGL_30_API
        
        NSOpenGLPFAOpenGLProfile,
        NSOpenGLProfileVersion3_2Core,
        
#endif
        0
    };
    
    class graphics_context_osx : public graphics_context
    {
    private:
        
    protected:
        
        NSOpenGLContext *m_context;
        
    public:
        
        graphics_context_osx(const std::shared_ptr<ogl_window>& window);
        ~graphics_context_osx();
        
        void* get_context() const;
        
        void make_current();
        void draw() const;
    };
    
    std::shared_ptr<graphics_context> create_graphics_context_osx(const std::shared_ptr<ogl_window>& window)
    {
        assert(window != nullptr);
        return std::make_shared<graphics_context_osx>(window);
    };
    
    graphics_context_osx::graphics_context_osx(const std::shared_ptr<ogl_window>& window)
    {
        m_window = window;
        
        NSOpenGLPixelFormat *pixel_format = [[NSOpenGLPixelFormat alloc] initWithAttributes:g_attributes];
        if (!pixel_format)
        {
            assert(false);
        }
        
        m_context = [[NSOpenGLContext alloc] initWithFormat:pixel_format shareContext:[NSOpenGLContext currentContext]];
        NSOpenGLView *view = (__bridge NSOpenGLView*)m_window->get_hwnd();
        [view setPixelFormat:pixel_format];
        [view setOpenGLContext:m_context];
        [m_context makeCurrentContext];
        
        GLint swap = 1;
        [m_context setValues:&swap forParameter:NSOpenGLCPSwapInterval];
        
#if USED_GRAPHICS_API == OPENGL_30_API
        
        CGLEnable([m_context CGLContextObj], kCGLCECrashOnRemovedFunctions);
        
#endif
        
        i32 binded_frame_buffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &binded_frame_buffer);
        m_frame_buffer = binded_frame_buffer;
        
        i32 binded_render_buffer = 0;
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &binded_render_buffer);
        m_render_buffer = binded_render_buffer;
    }
    
    graphics_context_osx::~graphics_context_osx()
    {
        
    }
    
    void* graphics_context_osx::get_context() const
    {
        return (__bridge void *)m_context;
    }
    
    void graphics_context_osx::make_current()
    {
        graphics_context::make_current();
        [m_context makeCurrentContext];
    }
    
    void graphics_context_osx::draw(void) const
    {
        CGLFlushDrawable([m_context CGLContextObj]);
    }
}
#endif
