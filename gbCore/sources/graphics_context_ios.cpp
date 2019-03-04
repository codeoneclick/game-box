//
//  graphics_context_ios.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "graphics_context.h"

#if defined(__IOS__) && USED_GRAPHICS_API != NO_GRAPHICS_API

#include "window_impl.h"
#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>

namespace gb
{
    class graphics_context_ios : public graphics_context
    {
    private:
        
        bool m_msaa_enabled;
        
    protected:
        
        EAGLContext* m_context;
        
    public:
        
        graphics_context_ios(const std::shared_ptr<window_impl>& window);
        ~graphics_context_ios();
        
        void* get_context() const;
        
        ui32 get_frame_buffer() const;
        ui32 get_render_buffer() const;
        
        void make_current();
        void draw() const;
    };
    
    std::shared_ptr<graphics_context> create_graphics_context_ios(const std::shared_ptr<window_impl>& window)
    {
        return std::make_shared<graphics_context_ios>(window);
    };
    
    graphics_context_ios::graphics_context_ios(const std::shared_ptr<window_impl>& window) :
    m_msaa_enabled(false)
    {
        m_window = window;
        
        const UIView* hwnd = (__bridge UIView*)m_window->get_hwnd();
        
#if USED_GRAPHICS_API == OPENGL_30_API
        
        m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES3];
        
#elif USED_GRAPHICS_API == OPENGL_20_API
        
        m_context = [[EAGLContext alloc] initWithAPI:kEAGLRenderingAPIOpenGLES2];
        
#endif
        assert(m_context != nullptr);
        
        ui8 result = [EAGLContext setCurrentContext:m_context];
        assert(result == true);
        
        gl::command::create_render_buffers(1, &m_render_buffer);
        gl::command::bind_render_buffer(gl::constant::render_buffer, m_render_buffer);
        [m_context renderbufferStorage:gl::constant::render_buffer fromDrawable:static_cast<CAEAGLLayer*>(hwnd.layer)];
        
        GLint width;
        GLint height;
        glGetRenderbufferParameteriv(gl::constant::render_buffer, gl::constant::render_buffer_width, &width);
        glGetRenderbufferParameteriv(gl::constant::render_buffer, gl::constant::render_buffer_height, &height);
        
        gl::command::create_frame_buffers(1, &m_frame_buffer);
        gl::command::bind_frame_buffer(gl::constant::frame_buffer, m_frame_buffer);
        gl::command::attach_frame_buffer_render_buffer(gl::constant::frame_buffer, gl::constant::color_attachment_0, gl::constant::render_buffer, m_render_buffer);
        
        if(m_msaa_enabled)
        {
            GLint max_samples_allowed;
            
#if USED_GRAPHICS_API == OPENGL_30_API
            
             glGetIntegerv(GL_MAX_SAMPLES, &max_samples_allowed);
            
#elif USED_GRAPHICS_API == OPENGL_20_API
            
             glGetIntegerv(GL_MAX_SAMPLES_APPLE, &max_samples_allowed);
            
#endif
            
            gl::command::create_frame_buffers(1, &m_msaa_frame_buffer);
            gl::command::bind_frame_buffer(gl::constant::frame_buffer, m_msaa_frame_buffer);
            
            gl::command::create_render_buffers(1, &m_msaa_render_buffer);
            gl::command::bind_render_buffer(gl::constant::render_buffer, m_msaa_render_buffer);
            
#if USED_GRAPHICS_API == OPENGL_30_API
            
            glRenderbufferStorageMultisample(gl::constant::render_buffer, max_samples_allowed, GL_RGBA8 , width, height);
            
#elif USED_GRAPHICS_API == OPENGL_20_API
            
            glRenderbufferStorageMultisampleAPPLE(gl::constant::render_buffer, max_samples_allowed, GL_RGBA8_OES , width, height);
            
#endif
            
            gl::command::attach_frame_buffer_render_buffer(gl::constant::frame_buffer, gl::constant::color_attachment_0, gl::constant::render_buffer, m_msaa_render_buffer);
        }
        assert(gl::command::check_frame_buffer_status(gl::constant::frame_buffer) == gl::constant::frame_buffer_complete);
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
        if(m_msaa_enabled)
        {
            return m_msaa_frame_buffer;
        }
        else
        {
            return m_frame_buffer;
        }
    }
    
    ui32 graphics_context_ios::get_render_buffer() const
    {
        if(m_msaa_enabled)
        {
            return m_msaa_render_buffer;
        }
        else
        {
            return m_render_buffer;
        }
    }
    
    void graphics_context_ios::make_current()
    {
        graphics_context::make_current();
        ui8 result = [EAGLContext setCurrentContext:m_context];
        assert(result == true);
    }
    
    void graphics_context_ios::draw() const
    {
        if(m_msaa_enabled)
        {
#if USED_GRAPHICS_API == OPENGL_30_API
            
            
#elif USED_GRAPHICS_API == OPENGL_20_API
            
            gl::command::bind_frame_buffer(GL_READ_FRAMEBUFFER_APPLE, m_msaa_frame_buffer);
            gl::command::bind_frame_buffer(GL_DRAW_FRAMEBUFFER_APPLE, m_frame_buffer);
            glResolveMultisampleFramebufferAPPLE();
            
#endif
            
        }
        
        assert(m_context != nullptr);
        gl::command::bind_render_buffer(gl::constant::render_buffer, m_render_buffer);
        [m_context presentRenderbuffer:gl::constant::render_buffer];
    }
}
#endif
