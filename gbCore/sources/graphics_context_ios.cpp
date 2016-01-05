//
//  graphics_context_ios.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "graphics_context.h"
#include "ogl_window.h"

#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>

namespace gb
{
    class graphics_context_ios : public graphics_context
    {
    private:
        
    protected:
        
        EAGLContext* m_context;
        
    public:
        
        graphics_context_ios(const std::shared_ptr<ogl_window>& window);
        ~graphics_context_ios(void);
        
        void* get_context(void) const;
        
        void make_current(void) const;
        void draw(void) const;
    };
    
    std::shared_ptr<graphics_context> create_graphics_context_ios(const std::shared_ptr<ogl_window>& window)
    {
        return std::make_shared<graphics_context_ios>(window);
    };
    
    graphics_context_ios::graphics_context_ios(const std::shared_ptr<ogl_window>& window)
    {
        m_window = window;
        
        const UIView* hwnd = (__bridge UIView*)m_window->get_hwnd();
        
#if defined(__OPENGL_30__)
        m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
#else
        m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
#endif
        assert(m_context != nullptr);
        
        ui8 result = [EAGLContext setCurrentContext:m_context];
        assert(result == true);
        
        gl_create_render_buffers(1, &m_render_buffer);
        gl_bind_render_buffer(GL_RENDERBUFFER, m_render_buffer);
        [m_context renderbufferStorage:GL_RENDERBUFFER fromDrawable:static_cast<CAEAGLLayer*>(hwnd.layer)];
        
        gl_create_frame_buffers(1, &m_frame_buffer);
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_frame_buffer);
        gl_attach_frame_buffer_render_buffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_render_buffer);
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    }
    
    graphics_context_ios::~graphics_context_ios(void)
    {
        
    }
    
    void* graphics_context_ios::get_context(void) const
    {
        return (__bridge void *)m_context;
    }
    
    void graphics_context_ios::make_current(void) const
    {
        ui8 result = [EAGLContext setCurrentContext:m_context];
        assert(result == true);
    }
    
    void graphics_context_ios::draw(void) const
    {
        assert(m_context != nullptr);
        [m_context presentRenderbuffer:GL_RENDERBUFFER];
    }
}
