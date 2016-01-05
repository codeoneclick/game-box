//
//  render_technique_ss.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef render_technique_ss_h
#define render_technique_ss_h

#include "render_technique_base.h"

namespace gb
{
    class render_technique_ss : public render_technique_base
    {
    private:
        
    protected:
        
        texture_shared_ptr m_color_attachment_texture;
        material_shared_ptr m_material;
        mesh_shared_ptr m_quad;
        
    public:
        
        render_technique_ss(ui32 width, ui32 height, const std::string& name, const material_shared_ptr& material);
        ~render_technique_ss(void);
        
        texture_shared_ptr get_color_attachment_texture(void) const;
        material_shared_ptr get_material(void) const;
        
        void bind(void);
        void unbind(void);
        void draw(void);
    };
};

#endif
