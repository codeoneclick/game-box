//
//  graphics_context_tvos.cpp
//  gbCore
//
//  Created by serhii serhiiv on 4/5/17.
//  Copyright © 2017 sergey.sergeev. All rights reserved.
//

#include "graphics_context.h"

#if defined(__TVOS__) && USED_GRAPHICS_API != NO_GRAPHICS_API

#include "window_impl.h"
#include <Foundation/Foundation.h>
#include <UIKit/UIKit.h>
#include <QuartzCore/QuartzCore.h>

namespace gb
{
    class graphics_context_tvos : public graphics_context
    {
    private:
        
    protected:
        
        EAGLContext* m_context;
        
    public:
        
        graphics_context_tvos(const std::shared_ptr<window_impl>& window);
        ~graphics_context_tvos();
        
        void* get_context() const;
        
        void make_current();
        void draw() const;
    };
    
    std::shared_ptr<graphics_context> create_graphics_context_tvos(const std::shared_ptr<window_impl>& window)
    {
        return std::make_shared<graphics_context_tvos>(window);
    };
    
    graphics_context_tvos::graphics_context_tvos(const std::shared_ptr<window_impl>& window)
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
        
        gl::command::create_frame_buffers(1, &m_frame_buffer);
        gl::command::bind_frame_buffer(gl::constant::frame_buffer, m_frame_buffer);
        gl::command::attach_frame_buffer_render_buffer(gl::constant::frame_buffer, gl::constant::color_attachment_0, gl::constant::render_buffer, m_render_buffer);
        assert(gl::command::check_frame_buffer_status(gl::constant::frame_buffer) == gl::constant::frame_buffer_complete);
    }
    
    graphics_context_tvos::~graphics_context_tvos()
    {
        
    }
    
    void* graphics_context_tvos::get_context() const
    {
        return (__bridge void *)m_context;
    }
    
    void graphics_context_tvos::make_current()
    {
        graphics_context::make_current();
        ui8 result = [EAGLContext setCurrentContext:m_context];
        assert(result == true);
    }
    
    void graphics_context_tvos::draw() const
    {
        assert(m_context != nullptr);
        [m_context presentRenderbuffer:gl::constant::render_buffer];
    }
}
#endif
