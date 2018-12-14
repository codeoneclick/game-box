//
//  render_target.h
//  gbCore
//
//  Created by sergey.sergeev on 11/6/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "main_headers.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

#include "declarations.h"

namespace gb
{
    class render_target
    {
    private:
        
    protected:
        
        ui32 m_frame_buffer;
        ui32 m_pixel_buffer;
        ui32 m_color_attachment;
        ui32 m_depth_attachment;
        
        glm::ivec2 m_size;
        GLint m_format;
        
        bool m_is_custom_color_attachment;
        
    public:
        
        render_target(GLint format, const glm::ivec2& size, const texture_shared_ptr& custom_attachment = nullptr);
        ~render_target();
        
        void clear();
        void begin();
        void end(ui8* data = nullptr);
        void grab(ui8* data);
        
        texture_shared_ptr grab_color_attachment();
    };
};

#endif
