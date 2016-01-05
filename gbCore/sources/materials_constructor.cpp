//
//  materials_constructor.cpp
//  gbCore
//
//  Created by sergey.sergeev on 12/22/15.
//  Copyright © 2015 sergey.sergeev. All rights reserved.
//

#include "materials_constructor.h"
#include "built_in_shaders.h"
#include "shader.h"
#include "material.h"

namespace gb
{
    std::once_flag g_direction_light_shader_created;
    std::once_flag g_omni_light_shader_created;
    std::once_flag g_wireframe_grid_shader_created;
    std::once_flag g_3d_cursor_shader_created;
    
    shader_shared_ptr materials_constructor::get_direction_light_shader()
    {
        static shader_shared_ptr shader = nullptr;
        std::call_once(g_direction_light_shader_created, [] {
            shader = shader::construct("direction_light_shader",
                                       shader_direction_light_vert,
                                       shader_direction_light_frag);
            assert(shader);
        });
        return shader;
    }
    
    shader_shared_ptr materials_constructor::get_omni_light_shader()
    {
        static shader_shared_ptr shader = nullptr;
        std::call_once(g_omni_light_shader_created, [] {
            shader = shader::construct("omni_light_shader",
                                       shader_omni_light_vert,
                                       shader_omni_light_frag);
            assert(shader);
        });
        return shader;
    }
    
    shader_shared_ptr materials_constructor::get_wireframe_grid_shader()
    {
        static shader_shared_ptr shader = nullptr;
        std::call_once(g_wireframe_grid_shader_created, [] {
            shader = shader::construct("wireframe_grid_shader",
                                       shader_wireframe_grid_vert,
                                       shader_wireframe_grid_frag);
            assert(shader);
        });
        return shader;
    }
    
    shader_shared_ptr materials_constructor::get_3d_cursor_shader()
    {
        static shader_shared_ptr shader = nullptr;
        std::call_once(g_3d_cursor_shader_created, [] {
            shader = shader::construct("3d_cursor_shader",
                                       shader_3d_cursor_vert,
                                       shader_3d_cursor_frag);
            assert(shader);
        });
        return shader;
    }
    
    material_shared_ptr materials_constructor::create_direction_light_material()
    {
        material_shared_ptr material = std::make_shared<gb::material>();
        material->set_shader(materials_constructor::get_direction_light_shader());
        
        material->set_culling(false);
        material->set_culling_mode(GL_BACK);
        
        material->set_blending(true);
        material->set_blending_function_source(GL_ONE);
        material->set_blending_function_destination(GL_ONE);
        
        material->set_stencil_test(false);
        material->set_stencil_function(GL_ALWAYS);
        material->set_stencil_function_parameter_1(1);
        material->set_stencil_function_parameter_2(255);
        material->set_stencil_mask_parameter(255);
        
        material->set_depth_test(false);
        material->set_depth_mask(false);
        
        material->set_clipping(false);
        material->set_clipping_plane(glm::vec4(0.f));
        
        material->set_reflecting(false);
        material->set_shadowing(false);
        material->set_debugging(false);
        
        return material;
    }
    
    material_shared_ptr materials_constructor::create_omni_light_material()
    {
        material_shared_ptr material = std::make_shared<gb::material>();
        material->set_shader(materials_constructor::get_omni_light_shader());
        
        material->set_culling(false);
        material->set_culling_mode(GL_BACK);
        
        material->set_blending(true);
        material->set_blending_function_source(GL_ONE);
        material->set_blending_function_destination(GL_ONE);
        
        material->set_stencil_test(false);
        material->set_stencil_function(GL_ALWAYS);
        material->set_stencil_function_parameter_1(1);
        material->set_stencil_function_parameter_2(255);
        material->set_stencil_mask_parameter(255);
        
        material->set_depth_test(false);
        material->set_depth_mask(false);
        
        material->set_clipping(false);
        material->set_clipping_plane(glm::vec4(0.f));
        
        material->set_reflecting(false);
        material->set_shadowing(false);
        material->set_debugging(false);
        
        return material;
    }
    
    material_shared_ptr materials_constructor::create_wireframe_grid_material()
    {
        material_shared_ptr material = std::make_shared<gb::material>();
        material->set_shader(materials_constructor::get_wireframe_grid_shader());
        
        material->set_culling(true);
        material->set_culling_mode(GL_BACK);
        
        material->set_blending(false);
        material->set_blending_function_source(GL_SRC_ALPHA);
        material->set_blending_function_destination(GL_ONE_MINUS_SRC_ALPHA);
        
        material->set_stencil_test(false);
        material->set_stencil_function(GL_ALWAYS);
        material->set_stencil_function_parameter_1(1);
        material->set_stencil_function_parameter_2(255);
        material->set_stencil_mask_parameter(255);
        
        material->set_depth_test(true);
        material->set_depth_mask(true);
        
        material->set_clipping(false);
        material->set_clipping_plane(glm::vec4(0.f));
        
        material->set_reflecting(false);
        material->set_shadowing(false);
        material->set_debugging(false);
        
        material->set_z_order(99);
        
        return material;
    }
    
    material_shared_ptr materials_constructor::create_3d_cursor_material()
    {
        material_shared_ptr material = std::make_shared<gb::material>();
        material->set_shader(materials_constructor::get_3d_cursor_shader());
        
        material->set_culling(true);
        material->set_culling_mode(GL_FRONT);
        
        material->set_blending(true);
        material->set_blending_function_source(GL_SRC_ALPHA);
        material->set_blending_function_destination(GL_ONE_MINUS_SRC_ALPHA);
        
        material->set_stencil_test(false);
        material->set_stencil_function(GL_ALWAYS);
        material->set_stencil_function_parameter_1(1);
        material->set_stencil_function_parameter_2(255);
        material->set_stencil_mask_parameter(255);
        
        material->set_depth_test(true);
        material->set_depth_mask(true);
        
        material->set_clipping(false);
        material->set_clipping_plane(glm::vec4(0.f));
        
        material->set_reflecting(false);
        material->set_shadowing(false);
        material->set_debugging(false);
        
        material->set_z_order(100);
        
        return material;
    }
}