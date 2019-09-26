//
//  graphics_context.cpp
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#include "graphics_context.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "window_impl.h"

namespace gb
{
    extern std::shared_ptr<graphics_context> create_graphics_context_ios(const std::shared_ptr<window_impl>& window);
    extern std::shared_ptr<graphics_context> create_graphics_context_osx(const std::shared_ptr<window_impl>& window);
	extern std::shared_ptr<graphics_context> create_graphics_context_win32(const std::shared_ptr<window_impl>& window);
    extern std::shared_ptr<graphics_context> create_graphics_context_tvos(const std::shared_ptr<window_impl>& window);

    graphics_context_shared_ptr graphics_context::m_current_context = nullptr;
    
    graphics_context_shared_ptr graphics_context::construct(const std::shared_ptr<window_impl> &window, gb::e_graphic_context_api api)
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

			case e_graphic_context_api_win32:
			{

#if defined(__WINOS__)

				context = create_graphics_context_win32(window);
#else

				assert(false);

#endif
			}
                break;
                
            case e_graphic_context_api_tvos:
            {
#if defined(__TVOS__)

                context = create_graphics_context_tvos(window);

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
		m_current_context = context;
        return context;
    }
    
    ui32 graphics_context::get_frame_buffer() const
    {
        return m_frame_buffer;
    }
    
    ui32 graphics_context::get_render_buffer() const
    {
        return m_render_buffer;
    }
    
    ui32 graphics_context::get_depth_buffer() const
    {
        return m_depth_buffer;
    }
    
    ui32 graphics_context::get_width() const
    {
        return m_window->get_resolution_size_in_pixels().x;
    }
    
    ui32 graphics_context::get_height() const
    {
        return m_window->get_resolution_size_in_pixels().y;
    }
    
    void graphics_context::make_current()
    {
        m_current_context = shared_from_this();
    }
    
    graphics_context_shared_ptr graphics_context::get_current_context()
    {
        return m_current_context;
    }
}

#endif
