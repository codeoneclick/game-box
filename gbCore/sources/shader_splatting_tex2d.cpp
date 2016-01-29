//
//  shader_splatting_tex2d.cpp
//  gbCore
//
//  Created by sergey.sergeev on 1/29/16.
//  Copyright © 2016 sergey.sergeev. All rights reserved.
//

#include "main_headers.h"

const char* shader_splatting_tex2d_vert = string_shader
(
#if defined(__OPENGL_30__)
 
 out vec2 v_texcoord;
 out vec2 v_scaled_texcoord;
 
#else
 
 varying vec2 v_texcoord;
 varying vec2 v_scaled_texcoord;
 
#endif
 
 const float k_texcoord_scale = 32.0;
 
 void main(void)
{
    v_texcoord = a_texcoord;
    v_scaled_texcoord = v_texcoord * k_texcoord_scale;
    gl_Position = vec4(a_position, 0.0, 1.0);
}
 );

const char* shader_splatting_tex2d_frag = string_shader
(
#if defined(__OPENGL_30__)
 
 in vec2 v_texcoord;
 in vec2 v_scaled_texcoord;
 
#else
 
 varying vec2 v_texcoord;
 varying vec2 v_scaled_texcoord;
 
#endif
 
 uniform sampler2D sampler_01;
 uniform sampler2D sampler_02;
 uniform sampler2D sampler_03;
 uniform sampler2D sampler_04;
 
 void main()
{
    vec4 mask_color = texture2D(sampler_04, v_texcoord);
    vec4 color = vec4(0.0, 0.0, 0.0, 1.0);
    
    if(mask_color.x > 0.0)
    {
        color = color + texture2D(sampler_01, v_scaled_texcoord) * mask_color.x;
    }
    if(mask_color.y > 0.0)
    {
        color = color + texture2D(sampler_02, v_scaled_texcoord) * mask_color.y;
    }
    if(mask_color.z > 0.0)
    {
        color = color + texture2D(sampler_03, v_scaled_texcoord) * mask_color.z;
    }
    
    gl_FragColor = vec4(1.0, 0.0, 0.0, 1.0);
}
 );
