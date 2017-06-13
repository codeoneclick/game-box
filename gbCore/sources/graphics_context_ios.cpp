//
//  graphics_context_ios.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//
#if defined(__IOS__) && !defined(__NO_RENDER__)

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
        ~graphics_context_ios();
        
        void* get_context() const;
        
        ui32 get_frame_buffer() const;
        ui32 get_render_buffer() const;
        
        void make_current();
        void draw() const;
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
        
        GLint width;
        GLint height;
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_WIDTH, &width);
        glGetRenderbufferParameteriv(GL_RENDERBUFFER, GL_RENDERBUFFER_HEIGHT, &height);
        
        gl_create_frame_buffers(1, &m_frame_buffer);
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_frame_buffer);
        gl_attach_frame_buffer_render_buffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_render_buffer);
        
        GLint max_samples_allowed;
        glGetIntegerv(GL_MAX_SAMPLES_APPLE, &max_samples_allowed);
        
        gl_create_frame_buffers(1, &m_msaa_frame_buffer);
        gl_bind_frame_buffer(GL_FRAMEBUFFER, m_msaa_frame_buffer);

        gl_create_render_buffers(1, &m_msaa_render_buffer);
        gl_bind_render_buffer(GL_RENDERBUFFER, m_msaa_render_buffer);
        
        glRenderbufferStorageMultisampleAPPLE(GL_RENDERBUFFER, max_samples_allowed, GL_RGBA8_OES , width, height);
        gl_attach_frame_buffer_render_buffer(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_RENDERBUFFER, m_msaa_render_buffer);
        
        assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE);
    }
    
    graphics_context_ios::~graphics_context_ios()
    {
        
    }
    
    void* graphics_context_ios::get_context() const
    {
        return (__bridge void *)m_context;
    }
    
    ui32 graphics_context_ios::get_frame_buffer() const
    {
        return m_msaa_frame_buffer;
    }
    
    ui32 graphics_context_ios::get_render_buffer() const
    {
        return m_msaa_render_buffer;
    }
    
    void graphics_context_ios::make_current()
    {
        graphics_context::make_current();
        ui8 result = [EAGLContext setCurrentContext:m_context];
        assert(result == true);
    }
    
    void graphics_context_ios::draw() const
    {
        gl_bind_frame_buffer(GL_READ_FRAMEBUFFER_APPLE, m_msaa_frame_buffer);
        gl_bind_frame_buffer(GL_DRAW_FRAMEBUFFER_APPLE, m_frame_buffer);
        glResolveMultisampleFramebufferAPPLE();

        assert(m_context != nullptr);
        gl_bind_render_buffer(GL_RENDERBUFFER, m_render_buffer);
        [m_context presentRenderbuffer:GL_RENDERBUFFER];
    }
}
#endif
