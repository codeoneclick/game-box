//
//  graphics_context.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "graphics_context.h"
#include "ogl_window.h"

namespace gb
{
    extern std::shared_ptr<graphics_context> create_graphics_context_ios(const std::shared_ptr<ogl_window>& window);
    extern std::shared_ptr<graphics_context> create_graphics_context_osx(const std::shared_ptr<ogl_window>& window);

    std::shared_ptr<graphics_context> graphics_context::construct(const std::shared_ptr<ogl_window> &window, gb::e_graphic_context_api api)
    {
        std::shared_ptr<graphics_context> context = nullptr;
        switch (api)
        {
            case e_graphic_context_api_osx:
            {
#if defined(__OSX__)
                context = create_graphics_context_osx(window);
#else
                assert(false);
#endif
            }
                break;
                
            case e_graphic_context_api_ios:
            {
#if defined(__IOS__)
                context = create_graphics_context_ios(window);
#else
                assert(false);
#endif
            }
                break;
                
            default:
            {
                assert(false);
            }
                break;
        }
        assert(context != nullptr);
        return context;
    }
    
    ui32 graphics_context::get_frame_buffer(void) const
    {
        return m_frame_buffer;
    }
    
    ui32 graphics_context::get_render_buffer(void) const
    {
        return m_render_buffer;
    }
    
    ui32 graphics_context::get_depth_buffer(void) const
    {
        return m_depth_buffer;
    }
    
    ui32 graphics_context::get_width(void) const
    {
        return m_window->get_width();
    }
    
    ui32 graphics_context::get_height(void) const
    {
        return m_window->get_height();
    }
}