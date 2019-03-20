//
//  render_technique_ss.h
//  gbCore
//
//  Created by sergey.sergeev on 8/17/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#pragma once

#include "render_technique_base.h"
#include "ss_technique_configuration.h"

#if USED_GRAPHICS_API != NO_GRAPHICS_API

namespace gb
{
    class render_technique_ss : public render_technique_base
    {
    private:
        
    protected:
        
        texture_shared_ptr m_color_attachment_texture;
        material_shared_ptr m_material;
        mesh_2d_shared_ptr m_quad;
        
    public:
        
        render_technique_ss(ui32 width, ui32 height, const std::string& name);
		~render_technique_ss();
        
        static render_technique_ss_shared_ptr construct(const std::shared_ptr<ss_technique_configuration>& configuration,
                                                        const material_shared_ptr& material);
        
        texture_shared_ptr get_color_attachment_texture() const;
        material_shared_ptr get_material() const;
        
        void bind();
        void unbind();
        void draw();
    };
};

#endif
