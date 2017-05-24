//
//  render_technique_ss.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#if !defined(__NO_RENDER__)

#include "render_technique_base.h"
#include "mesh_2d.h"

namespace gb
{
    class render_technique_ss : public render_technique_base
    {
    private:
        
    protected:
        
        texture_shared_ptr m_color_attachment_texture;
        material_shared_ptr m_material;
        std::shared_ptr<mesh_2d<vertex_attribute>> m_quad;
        
    public:
        
        render_technique_ss(ui32 width, ui32 height, const std::string& name, const material_shared_ptr& material);
		~render_technique_ss();
        
        texture_shared_ptr get_color_attachment_texture() const;
        material_shared_ptr get_material() const;
        
        void bind();
        void unbind();
        void draw();
    };
};

#endif
