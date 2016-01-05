//
//  graphics_context.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef graphics_context_h
#define graphics_context_h

#include "main_headers.h"

namespace gb
{
    enum e_graphic_context_api
    {
        e_graphic_context_api_osx = 1,
        e_graphic_context_api_ios
    };
    
    class ogl_window;
    class graphics_context
    {
    private:
        
    protected:
        
        ui32 m_frame_buffer;
        ui32 m_render_buffer;
        ui32 m_depth_buffer;
        
        std::shared_ptr<ogl_window> m_window;
        
        graphics_context(void) = default;
        
    public:
       
        static std::shared_ptr<graphics_context> construct(const std::shared_ptr<ogl_window>& window,
                                                           e_graphic_context_api api);
        
        virtual ~graphics_context(void) = default;
        
        ui32 get_frame_buffer(void) const;
        ui32 get_render_buffer(void) const;
        ui32 get_depth_buffer(void) const;
        
        ui32 get_width(void) const;
        ui32 get_height(void) const;
        
        virtual void* get_context(void) const = 0;
        
        virtual void make_current(void) const = 0;
        virtual void draw(void) const = 0;
    };
};


#endif
