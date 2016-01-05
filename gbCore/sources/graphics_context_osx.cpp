//
//  graphics_context_osx.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "graphics_context.h"
#include "ogl_window.h"

#if defined(__OSX__)

#include <Cocoa/Cocoa.h>

#endif

namespace gb
{
    static NSOpenGLPixelFormatAttribute g_attributes[] =
    {
        NSOpenGLPFADoubleBuffer,
        NSOpenGLPFADepthSize, 24,
#if defined(__OPENGL_30__)
        
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
        ~graphics_context_osx(void);
        
        void* get_context(void) const;
        
        void make_current(void) const;
        void draw(void) const;
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
        
#if defined(__OPENGL_30__)
        
        CGLEnable([m_context CGLContextObj], kCGLCECrashOnRemovedFunctions);
        
#endif
        
        i32 binded_frame_buffer = 0;
        glGetIntegerv(GL_FRAMEBUFFER_BINDING, &binded_frame_buffer);
        m_frame_buffer = binded_frame_buffer;
        
        i32 binded_render_buffer = 0;
        glGetIntegerv(GL_RENDERBUFFER_BINDING, &binded_render_buffer);
        m_render_buffer = binded_render_buffer;
    }
    
    graphics_context_osx::~graphics_context_osx(void)
    {
        
    }
    
    void* graphics_context_osx::get_context(void) const
    {
        return (__bridge void *)m_context;
    }
    
    void graphics_context_osx::make_current(void) const
    {
        [m_context makeCurrentContext];
    }
    
    void graphics_context_osx::draw(void) const
    {
        CGLFlushDrawable([m_context CGLContextObj]);
    }
}
