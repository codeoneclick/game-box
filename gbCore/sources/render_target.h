//
//  render_target.h
//  gbCore
//
//  Created by sergey.sergeev on 11/6/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#ifndef render_target_h
#define render_target_h

#include "main_headers.h"
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
        
        std::shared_ptr<graphics_context> m_graphics_context;
        
    public:
        
        render_target(const std::shared_ptr<graphics_context>& graphics_context, GLint format, ui32 width, ui32 height);
        ~render_target();
        
        void clear();
        void begin();
        void end(ui8* data = nullptr);
    };
};

#endif
