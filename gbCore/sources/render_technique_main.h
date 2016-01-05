//
//  render_technique_main.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef render_technique_main_h
#define render_technique_main_h

#include "render_technique_base.h"

namespace gb
{
    class render_technique_main : render_technique_base
    {
    private:
        
    protected:
        
        material_shared_ptr m_material;
        mesh_shared_ptr m_quad;
        ui32 m_render_buffer;
        
        
    public:
        
        render_technique_main(ui32 width, ui32 height, const material_shared_ptr& material, ui32 frame_buffer, ui32 render_buffer);
        ~render_technique_main();
        
        void bind();
        void unbind();
        void draw();
    };
};

#endif
