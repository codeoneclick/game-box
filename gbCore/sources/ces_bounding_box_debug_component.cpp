//
//  ces_bounding_box_debug_component.cpp
//  gbCore
//
//  Created by sergey.sergeev on 11/26/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "ces_bounding_box_debug_component.h"
#include "material.h"
#include "shader.h"

namespace gb
{
    std::once_flag g_bounding_box_debug_material_created;
    static material_shared_ptr g_default_material = nullptr;
    
    ces_bounding_box_debug_component::ces_bounding_box_debug_component()
    {
        std::call_once(g_bounding_box_debug_material_created, []{
            g_default_material = std::make_shared<material>();
            shader_shared_ptr shader = shader::construct("debug_bounding_box",
                                                         shader_debug_bounding_box_vert,
                                                         shader_debug_bounding_box_frag);
            assert(shader);
            g_default_material->set_shader(shader);
            g_default_material->set_culling(false);
            g_default_material->set_culling_mode(GL_BACK);
            
            g_default_material->set_blending(false);
            g_default_material->set_blending_function_source(GL_SRC_ALPHA);
            g_default_material->set_blending_function_destination(GL_ONE);
            
            g_default_material->set_stencil_test(false);
            g_default_material->set_stencil_function(GL_ALWAYS);
            g_default_material->set_stencil_function_parameter_1(1);
            g_default_material->set_stencil_function_parameter_2(255);
            g_default_material->set_stencil_mask_parameter(255);
            
            g_default_material->set_depth_test(false);
            g_default_material->set_depth_mask(false);
            
            g_default_material->set_clipping(false);
            g_default_material->set_clipping_plane(glm::vec4(0.f));
            
            g_default_material->set_reflecting(false);
            g_default_material->set_shadowing(false);
            g_default_material->set_debugging(false);
        });
        m_material = g_default_material;
    }
    
    ces_bounding_box_debug_component::~ces_bounding_box_debug_component()
    {
        
    }
}