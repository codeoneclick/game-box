//
//  graphics_context_osx.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "graphics_context.h"

#if defined(__OSX__) && USED_GRAPHICS_API != NO_GRAPHICS_API

#include "window_impl.h"
#include <Cocoa/Cocoa.h>

#if USED_GRAPHICS_API == METAL_API

#include "mtl_device.h"
#include <MetalKit/MetalKit.h>

#endif

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
        
        graphics_context_osx(const std::shared_ptr<window_impl>& window);
        ~graphics_context_osx();
        
        void* get_context() const;
        
        void make_current();
        void draw() const;
    };
    
    std::shared_ptr<graphics_context> create_graphics_context_osx(const std::shared_ptr<window_impl>& window)
    {
        assert(window != nullptr);
        return std::make_shared<graphics_context_osx>(window);
    };
    
    graphics_context_osx::graphics_context_osx(const std::shared_ptr<window_impl>& window)
    {
        m_window = window;
        
#if USED_GRAPHICS_API == OPENGL_20_API || USED_GRAPHICS_API == OPENGL_30_API
        
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
        glGetIntegerv(gl::constant::frame_buffer_binding, &binded_frame_buffer);
        m_frame_buffer = binded_frame_buffer;
        
        i32 binded_render_buffer = 0;
        glGetIntegerv(gl::constant::render_buffer_binding, &binded_render_buffer);
        m_render_buffer = binded_render_buffer;
        
#elif USED_GRAPHICS_API == METAL_API
        
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
        view.preferredFramesPerSecond = 60;
        view.sampleCount = 1;
        
#endif
        
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
        
#if USED_GRAPHICS_API == OPENGL_30_API || USED_GRAPHICS_API == OPENGL_20_API
        
        [m_context makeCurrentContext];
        
#elif USED_GRAPHICS_API == METAL_API
        
        mtl_device::get_instance()->bind();
        
#endif
        
    }
    
    void graphics_context_osx::draw(void) const
    {
    
#if USED_GRAPHICS_API == OPENGL_30_API || USED_GRAPHICS_API == OPENGL_20_API
        
        CGLFlushDrawable([m_context CGLContextObj]);
        
#elif USED_GRAPHICS_API == METAL_API
        
        mtl_device::get_instance()->unbind();
        
#endif
        
    }
}
#endif
