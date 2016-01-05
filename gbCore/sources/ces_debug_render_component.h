//
//  ces_debug_render_component.h
//  gbCore
//
//  Created by sergey.sergeev on 8/31/15.
//  Copyright (c) 2015 sergey.sergeev. All rights reserved.
//

#ifndef ces_debug_render_component_h
#define ces_debug_render_component_h

#include "ces_base_component.h"

namespace gb
{
    class ces_debug_render_component : public ces_base_component
    {
    private:
        
    protected:
        
        material_shared_ptr m_material;
        mesh_shared_ptr m_mesh;
        
        void bind_main_shader_uniforms(const material_shared_ptr& material);
        
    public:
        
        ces_debug_render_component();
        ~ces_debug_render_component();
        
        void on_bind();
        void on_draw();
        void on_unbind();
        
        void set_mesh(const mesh_shared_ptr& mesh);
        
        material_shared_ptr get_material() const;
    };
};

#endif
