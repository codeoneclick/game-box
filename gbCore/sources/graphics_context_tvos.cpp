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

#if USED_GRAPHICS_API == METAL_API

#include <MetalKit/MetalKit.h>
#include "mtl_device.h"

#endif

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
        
#if USED_GRAPHICS_API == OPENGL_30_API || USED_GRAPHICS_API == OPENGL_20_API
        
        assert(m_context != nullptr);
        
        ui8 result = [EAGLContext setCurrentContext:m_context];
        assert(result == true);
        
#endif
        
        gl::command::create_render_buffers(1, &m_render_buffer);
        gl::command::bind_render_buffer(gl::constant::render_buffer, m_render_buffer);
        
#if USED_GRAPHICS_API == OPENGL_30_API || USED_GRAPHICS_API == OPENGL_20_API
        
        [m_context renderbufferStorage:gl::constant::render_buffer fromDrawable:static_cast<CAEAGLLayer*>(hwnd.layer)];
        
#endif
        
        gl::command::create_frame_buffers(1, &m_frame_buffer);
        gl::command::bind_frame_buffer(gl::constant::frame_buffer, m_frame_buffer);
        gl::command::attach_frame_buffer_render_buffer(gl::constant::frame_buffer, gl::constant::color_attachment_0, gl::constant::render_buffer, m_render_buffer);
        assert(gl::command::check_frame_buffer_status(gl::constant::frame_buffer) == gl::constant::frame_buffer_complete);
        
#if USED_GRAPHICS_API == METAL_API
        
        mtl_device::get_instance()->init(m_window->get_hwnd());
        MTKView *view = (__bridge MTKView*)m_window->get_hwnd();
        
        if(!view.device)
        {
            NSLog(@"metal is not supported on this device");
            return;
        }
        
        view.colorPixelFormat = MTLPixelFormatBGRA8Unorm_sRGB;
        view.depthStencilPixelFormat = MTLPixelFormatDepth32Float_Stencil8;
        view.framebufferOnly = NO;
        
#endif
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
        
#if USED_GRAPHICS_API == OPENGL_30_API || USED_GRAPHICS_API == OPENGL_20_API
        
        ui8 result = [EAGLContext setCurrentContext:m_context];
        assert(result == true);
        
        
#elif USED_GRAPHICS_API == METAL_API
        
        mtl_device::get_instance()->bind();
        
#endif
        
    }
    
    void graphics_context_tvos::draw() const
    {
        
#if USED_GRAPHICS_API == OPENGL_30_API || USED_GRAPHICS_API == OPENGL_20_API
        
        assert(m_context != nullptr);
        [m_context presentRenderbuffer:gl::constant::render_buffer];
        
#elif USED_GRAPHICS_API == METAL_API
        
        mtl_device::get_instance()->unbind();
        
#endif
        
    }
}
#endif
