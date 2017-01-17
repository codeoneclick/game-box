//
//  render_technique_ws.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "render_technique_base.h"

namespace gb
{
    class render_technique_ws : public render_technique_base
    {
    private:
        
    protected:
        
        texture_shared_ptr m_color_attachment_texture;
        texture_shared_ptr m_depth_attachment_texture;
        
        i32 m_num_passes;
        
    public:
        
        render_technique_ws(ui32 width, ui32 height, const std::string& name, ui32 index,
                            bool is_depth_compare_mode_enabled, i32 num_passes);
        ~render_technique_ws();
        
        texture_shared_ptr get_color_attachment_texture() const;
        texture_shared_ptr get_depth_attachment_texture() const;
        
        void bind();
        void unbind();
        void draw();
        
        i32 get_num_passes() const;
    };
};

#endif
